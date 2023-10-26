#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <vector>
#include "matrix.h"
Matrix perspectiveTransform(double zView) {
	Matrix perspectiveMatrix(4, 4);
	perspectiveMatrix(3, 2) = 1 / zView;
	perspectiveMatrix(2, 2) = 1 / zView;
	return perspectiveMatrix;
}
Matrix screenTransform(int xs, int ys, double maxInt) {
	Matrix screenMatrix(4, 4);
	screenMatrix(0, 0) = xs / 2;
	screenMatrix(1, 1) = -1 * ys / 2;
	screenMatrix(2, 2) = maxInt;
	screenMatrix(0, 3) = xs / 2;
	screenMatrix(1, 3) = ys / 2;
	return screenMatrix;
}
const int width = 800;
const int height = 600;
const int depth = 600;

class  Color {
public:
	double r, g, b;
	Color operator*(const double k) const {//颜色的数乘
		Color result;
		result.r = r * k;
		result.g = g * k;
		result.b = b * k;
		return result;
	}
	Color operator+(const Color& other) const {//颜色相加
		Color result;
		result.r = r + other.r;
		result.g = g + other.g;
		result.b = b + other.b;
		return result;
	}
};
struct Point {
	double x, y, z;//坐标
	Color c;//颜色
	Point operator*(const int s)const {
		Point p0;
		p0.x = x * s;
		p0.y = y * s;
		p0.z = z * s;
		p0.c = c;
		return p0;
	}
};

struct Triangle {
	Point a, b, c;//三角形
};
struct Vector2
{
	double  x, y;
};
struct Vector3
{
	double  x, y, z;
};
struct Cube {
	Point point0, point1, point2, point3, point4, point5, point6, point7;
};

double cross2(double a, double b, double c, double d) {
	return a * d - b * c;
}
//判断是否在三角形内
bool inTriangle(Point* tranglePoins, double x, double y) {
	Point p1 = tranglePoins[0];
	Point p2 = tranglePoins[1];
	Point p3 = tranglePoins[2];
	double d1 = cross2(p1.x - p2.x, p1.y - p2.y, p2.x - x, p2.y - y);
	double d2 = cross2(p2.x - p3.x, p2.y - p3.y, p3.x - x, p3.y - y);
	double d3 = cross2(p3.x - p1.x, p3.y - p1.y, p1.x - x, p1.y - y);
	if ((d1 > 0 && d2 > 0 && d3 >= 0) || (d1 < 0 && d2 < 0 && d3 < 0))
	{
		return true;
	}
	return false;
}
//计算重心坐标
Vector3 middlePoint(Point* trianglePoints, double x, double y) {
	Point a = trianglePoints[0];
	Point b = trianglePoints[1];
	Point c = trianglePoints[2];
	double x0 = a.x;
	double y0 = a.y;
	double x1 = b.x;
	double y1 = b.y;
	double x2 = c.x;
	double y2 = c.y;
	double s = (-1 * (x - x1) * (y2 - y1) + (y - y1) * (x2 - x1)) /
		(-1 * (x0 - x1) * (y2 - y1) + (y0 - y1) * (x2 - x1));
	double t = (-1 * (x - x2) * (y0 - y2) + (y - y2) * (x0 - x2)) /
		(-1 * (x1 - x2) * (y0 - y2) + (y1 - y2) * (x0 - x2));
	Vector3 m;
	m.x = 0;
	m.y = 0;
	m.z = 0;
	if (s >= 0 && t >= 0 && (1 - s - t) >= 0) {
		m.x = s;
		m.y = t;
		m.z = 1 - s - t;
	}
	return m;
}
Triangle PointsToTriangle(Point* Points) {
	Point A = Points[0];
	Point B = Points[1];
	Point C = Points[2];
	Triangle Tre;
	Tre.a = A;
	Tre.b = B;
	Tre.c = C;
	return Tre;
}
//计算三角内的像素颜色
void drawTriangle(Point* A,Color** image) {
	int x1 = A[0].x;
	int x2 = A[1].x;
	int x3 = A[2].x;
	int y1 = A[0].y;
	int y2 = A[1].y;
	int y3 = A[2].y;
	int yLimitMin = std::min(A[0].y, std::min(A[1].y, A[2].y));
	int yLimitMax = std::max(A[0].y, std::max(A[1].y, A[2].y));
	int xLimitMin = std::min(A[0].x, std::min(A[1].x, A[2].x));
	int xLimitMax = std::max(A[0].x, std::max(A[1].x, A[2].x));
	for (int x = xLimitMin; x < xLimitMax; x++) {
		for (int y = yLimitMin; y < yLimitMax; y++) {
			if (inTriangle(A, x, y)) {
				Vector3 m = middlePoint(A, x, y);
				if (m.x >= 0 && m.x <= 1 && m.y >= 0 && m.y <= 1 && std::abs(m.x + m.y + m.z - 1.0) < 0.001) {
					image[x][y] = A[0].c * m.x + A[1].c * m.y + A[2].c * m.z;
				}
			}
		}
	}
}
void drawPlane(Triangle a, Color** image, Matrix finalMatrix) {
	Point Points[3];
	Points[0] = a.a;
	Points[1] = a.b;
	Points[2] = a.c;
	for (int i = 0; i < 3; i++) {//变换
		Matrix point = Matrix(4, 1);
		point(0, 0) = Points[i].x;
		point(1, 0) = Points[i].y;
		point(2, 0) = Points[i].z;
		point(3, 0) = 1.0;
		point = finalMatrix * point;//变换
		Points[i].x = point(0, 0) / point(3, 0);
		Points[i].y = point(1, 0) / point(3, 0);
		Points[i].z = point(2, 0) / point(3, 0);
	}
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			Vector3 m = middlePoint(Points, x, y);//质心坐标
			if (m.x >= 0 && m.x <= 1 && m.y >= 0 && m.y <= 1 && std::abs(m.x + m.y + m.z - 1.0) < 0.001) {
				double z = a.a.z * m.x
					+ a.b.z * m.y
					+ a.c.z * m.z;
				
				}
			}
		}
	}
void saveImage1(const char* filename1, Color** image) {
	FILE* fp = freopen(filename1, "w+", stdout);
	std::cout << "P6\n" << width << " " << height << "\n255\n";//声明格式
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			std::cout << (unsigned char)(image[x][y].r * 255) <<
				(unsigned char)(image[x][y].g * 255) << (unsigned char)(image[x][y].b * 255);
		}
	}
}
int main()
{
	Color Green = { 0,0.5,0 };
	Color Red = { 0.5,0,0 };
	Color Blue = { 0,0,0.5 };
	Color Yello = { 0,0.5,0.5 };
	Color Purple = { 0.5,0,0.5 };
	Color Cyan = { 0, 0.5, 0.5};
	Color Black = { 0,0,0 };
	Color White = { 0.5,0.5,0.5 };
	Cube Cube1 = { {0,0,0,Red},{1,0,0,Green},{1,1,0,Red},{0,1,0,Blue},
	{0,0,1,Yello},{1,0,1,Purple},{1,1,1,Cyan},{1,0,1,Black} };
	//初始化image
	Color** image = new Color * [width];
	for (int i = 0; i < width; ++i) {
		image[i] = new Color[height];//用new运算符分配了一个大小为 height 的 Color 数组，并将其赋值给 image[i]
		memset(image[i], 0, height * sizeof(Color));//动态分配内存
	}
	double** zBuffer = new double* [width];
	for (int i = 0; i < width; ++i) {
		zBuffer[i] = new double[height];
		memset(zBuffer[i], 0, height * sizeof(double));
		for (int j = 0; j < height; j++) {
			zBuffer[i][j] = 1e9;
		}
	}
	Point Points2[3] = {{50,100,0,Red},{100,300,0,Green},{200,100,0,Blue}};
	//drawPlane(Points3, image, transformMatrix);
	Triangle Points4 = { Cube1.point0, Cube1.point7, Cube1.point3 };
	//drawPlane(Points4, image, transformMatrix);


	//drawCube(Cube1, image, transformMatrix);
	drawTriangle(Points2, image);

	const char* filename = "projection4.ppm";
	saveImage1(filename, image);
	for (int i = 0; i < width; ++i) {
		delete[] image[i];
		delete[] zBuffer[i];
	}
	delete[] image;
	delete[] zBuffer;
	return 0;
}
