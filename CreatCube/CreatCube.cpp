#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <vector>
#include "matrix.h"
#include "Transfrom.h"
// 平移变换
Matrix Translate(double x, double y, double z, Matrix matrix) {
	Matrix translateMatrix(4, 4);
	translateMatrix(0, 3) = x;
	translateMatrix(1, 3) = y;
	translateMatrix(2, 3) = z;
	matrix = translateMatrix * matrix;
	return matrix;
}

// 旋转变换
Matrix Rotate(double angle, double x, double y, double z, Matrix matrix) {
	double cosTheta = cos(angle);
	double sinTheta = sin(angle);

	// 归一
	double length = sqrt(x * x + y * y + z * z);
	if (length != 0.0) {
		x /= length;
		y /= length;
		z /= length;
	}

	Matrix rotateMatrix(4, 4);
	rotateMatrix(0, 0) = x * x + (1 - x * x) * cosTheta;
	rotateMatrix(0, 1) = x * y * (1 - cosTheta) - z * sinTheta;
	rotateMatrix(0, 2) = x * z * (1 - cosTheta) + y * sinTheta;

	rotateMatrix(1, 0) = x * y * (1 - cosTheta) + z * sinTheta;
	rotateMatrix(1, 1) = y * y + (1 - y * y) * cosTheta;
	rotateMatrix(1, 2) = y * z * (1 - cosTheta) - x * sinTheta;

	rotateMatrix(2, 0) = x * z * (1 - cosTheta) - y * sinTheta;
	rotateMatrix(2, 1) = y * z * (1 - cosTheta) + x * sinTheta;
	rotateMatrix(2, 2) = z * z + (1 - z * z) * cosTheta;

	matrix = rotateMatrix * matrix;
	return matrix;

}

// 缩放变换
Matrix Scale(double scaleX, double scaleY, double scaleZ, Matrix matrix) {
	Matrix scaleMatrix(4, 4);
	scaleMatrix(0, 0) = scaleX;
	scaleMatrix(1, 1) = scaleY;
	scaleMatrix(2, 2) = scaleZ;
	scaleMatrix(3, 3) = 1.0;
	matrix = scaleMatrix * matrix;
	return matrix;
}
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
	double d;//深度区
	Color operator*(const double k) const {//颜色的数乘
		Color result;
		result.r = r * k;
		result.g = g * k;
		result.b = b * k;
		result.d = d * k;

		return result;
	}
	Color operator+(const Color& other) const {//颜色相加
		Color result;
		result.r = r + other.r;
		result.g = g + other.g;
		result.b = b + other.b;
		result.d = d + other.d;
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

std::vector<double> crossProduct(std::vector<double> a, std::vector<double> b) {//三维向量的叉乘定义
	std::vector<double> result(3);

	result[0] = a[1] * b[2] - a[2] * b[1];//新的x分量
	result[1] = a[2] * b[0] - a[0] * b[2];//新的y分量
	result[2] = a[0] * b[1] - a[1] * b[0];//新的z分量

	return result;

}
Matrix constructXiwMatrix(Point C, Point I, std::vector<double> up) {//C和I分别是相机坐标和相机朝向点的坐标
	//up是世界坐标的(0,1,0)
	Matrix Xiw(4, 4);//构造投影到相机平面的四维变换矩阵
	// 计算 Z 轴单位向量
	std::vector<double> Z = { I.x - C.x, I.y - C.y, I.z - C.z };
	double lengthZ = std::sqrt(Z[0] * Z[0] + Z[1] * Z[1] + Z[2] * Z[2]);
	Z[0] /= lengthZ;
	Z[1] /= lengthZ;
	Z[2] /= lengthZ;

	// 计算 Y 轴单位向量
	double dotProduct = up[0] * Z[0] + up[1] * Z[1] + up[2] * Z[2];
	std::vector<double> upPrime = { up[0] - dotProduct * Z[0], up[1] - dotProduct * Z[1], up[2] - dotProduct * Z[2] };//消除up向量在z向量上的投影
	//构造垂直于z的y轴向量upPrime
	double lengthY = std::sqrt(upPrime[0] * upPrime[0] + upPrime[1] * upPrime[1] + upPrime[2] * upPrime[2]);
	upPrime[0] /= lengthY;
	upPrime[1] /= lengthY;
	upPrime[2] /= lengthY;
	// 计算 X 轴单位向量
	std::vector<double> X = crossProduct(upPrime, Z);

	// 构造变换矩阵 Xiw
	Xiw(0, 0) = X[0];
	Xiw(0, 1) = X[1];
	Xiw(0, 2) = X[2];
	Xiw(0, 3) = -(X[0] * C.x + X[1] * C.y + X[2] * C.z);//可能行列顺序出问题

	Xiw(1, 0) = upPrime[0];
	Xiw(1, 1) = upPrime[1];
	Xiw(1, 2) = upPrime[2];
	Xiw(1, 3) = -(upPrime[0] * C.x + upPrime[1] * C.y + upPrime[2] * C.z);

	Xiw(2, 0) = Z[0];
	Xiw(2, 1) = Z[1];
	Xiw(2, 2) = Z[2];
	Xiw(2, 3) = -(Z[0] * C.x + Z[1] * C.y + Z[2] * C.z);

	Xiw(3, 0) = 0.0;
	Xiw(3, 1) = 0.0;
	Xiw(3, 2) = 0.0;
	Xiw(3, 3) = 1.0;
	return Xiw;

}

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
Triangle calculatePlane(Point* Points) {
	Point A = Points[0];
	Point B = Points[1];
	Point C = Points[2];

	Triangle plane;
	plane.a = A;
	plane.b = B;
	plane.c = C;
	return plane;

}
//画三角
void drawTriangle(Triangle a, Color** image, Color** image_buffer) {
	Point Points[3];
	Points[0] = a.a * 2;
	Points[1] = a.b * 2;
	Points[2] = a.c * 2;
	int yLimitMin = std::min(a.a.y, std::min(a.b.y, a.c.y));
	int yLimitMax = std::max(a.a.y, std::max(a.b.y, a.c.y));
	int xLimitMin = std::min(a.a.x, std::min(a.b.x, a.c.x));
	int xLimitMax = std::max(a.a.x, std::max(a.b.x, a.c.x));
	for (int i = 0; i <= 2; i++) {
		for (int j = 0; j <= 2; j++) {
			for (int y = yLimitMin * 2; y < yLimitMax * 2; ++y) {
				for (int x = xLimitMin * 2; x < xLimitMax * 2; ++x) {
					Color ans = { 0,0,0,0 };
					if (inTriangle(Points, x, y)) {
						Vector3 m = middlePoint(Points, x, y);
						if (m.x >= 0 && m.x <= 1 && m.y >= 0 && m.y <= 1 && std::abs(m.x + m.y + m.z - 1.0) < 0.001) {
							//std::cout << m.x + m.y + m.z;
							image_buffer[x][y].r = a.a.c.r * m.x + a.b.c.r * m.y + a.c.c.r * m.z;
							image_buffer[x][y].g = a.a.c.g * m.x + a.b.c.g * m.y + a.c.c.g * m.z;
							image_buffer[x][y].b = a.a.c.b * m.x + a.b.c.b * m.y + a.c.c.b * m.z;
						}
					}
					else {
						ans = ans + image_buffer[x][y];
					}

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
				if (z < image[(int)x][(int)y].d) {
					image[x][y].r = a.a.c.r * m.y + a.b.c.r * m.z + a.c.c.r * m.x;
					image[x][y].g = a.a.c.g * m.y + a.b.c.g * m.z + a.c.c.g * m.x;
					image[x][y].b = a.a.c.b * m.y + a.b.c.b * m.z + a.c.c.b * m.x;
					image[(int)x][(int)y].d = z;
				}
			}
		}
	}
}
void drawCube(Cube Cube1, Color** image, Matrix finalMatrix) {

	/*Point Points0[3] = {Cube1.point0,Cube1.point2,Cube1.point1};
	Point Points1[3] = { Cube1.point0,Cube1.point2,Cube1.point3 };//底面
	Point Points2[3] = { Cube1.point0,Cube1.point5,Cube1.point1 };
	Point Points3[3] = { Cube1.point0,Cube1.point5,Cube1.point4 };//前面
	Point Points4[3] = { Cube1.point0,Cube1.point7,Cube1.point3 };
	Point Points5[3] = { Cube1.point0,Cube1.point7,Cube1.point4 };//左面
	Point Points6[3] = { Cube1.point4,Cube1.point6,Cube1.point5 };
	Point Points7[3] = { Cube1.point4,Cube1.point6,Cube1.point7 };//上面
	Point Points8[3] = { Cube1.point2,Cube1.point7,Cube1.point3 };
	Point Points9[3] = { Cube1.point2,Cube1.point7,Cube1.point6 };//后面
	Point Points10[3] = { Cube1.point1,Cube1.point6,Cube1.point2 };
	Point Points11[3] = { Cube1.point1,Cube1.point6,Cube1.point5 };//右面
	Faces[0] = calculatePlane(Points0);
	Faces[1] = calculatePlane(Points1);
	Faces[2] = calculatePlane(Points2);
	Faces[3] = calculatePlane(Points3);
	Faces[4] = calculatePlane(Points4);
	Faces[5] = calculatePlane(Points5);
	Faces[6] = calculatePlane(Points6);
	Faces[7] = calculatePlane(Points7);
	Faces[8] = calculatePlane(Points8);
	Faces[9] = calculatePlane(Points9);
	Faces[10] = calculatePlane(Points10);
	Faces[11] = calculatePlane(Points11);*/
	Triangle Points0 = { Cube1.point0, Cube1.point2, Cube1.point1 };
	Triangle Points1 = { Cube1.point0, Cube1.point2, Cube1.point3 };//前面0123
	Triangle Points2 = { Cube1.point4, Cube1.point6, Cube1.point5 };
	Triangle Points3 = { Cube1.point4, Cube1.point6, Cube1.point7 };//后面4567
	Triangle Points4 = { Cube1.point0, Cube1.point7, Cube1.point3 };
	Triangle Points5 = { Cube1.point0, Cube1.point7, Cube1.point4 };//左面0734
	Triangle Points6 = { Cube1.point1, Cube1.point6, Cube1.point5 };
	Triangle Points7 = { Cube1.point1, Cube1.point6, Cube1.point2 };//右面1652
	Triangle Points8 = { Cube1.point2, Cube1.point7, Cube1.point6 };
	Triangle Points9 = { Cube1.point2, Cube1.point7, Cube1.point3 };//上面2763
	Triangle Points10 = { Cube1.point0, Cube1.point5, Cube1.point1 };
	Triangle Points11 = { Cube1.point0, Cube1.point5, Cube1.point4 };//下面0514
	drawPlane(Points0, image, finalMatrix);
	drawPlane(Points1, image, finalMatrix);
	drawPlane(Points2, image, finalMatrix);
	drawPlane(Points3, image, finalMatrix);
	drawPlane(Points4, image, finalMatrix);
	drawPlane(Points5, image, finalMatrix);
	drawPlane(Points6, image, finalMatrix);
	drawPlane(Points7, image, finalMatrix);
	drawPlane(Points8, image, finalMatrix);
	drawPlane(Points9, image, finalMatrix);
	drawPlane(Points10, image, finalMatrix);
	drawPlane(Points11, image, finalMatrix);

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
	Color Green = { 0,1,0 ,0 };
	Color Red = { 1,0,0 ,0 };
	Color Blue = { 0,0,1 ,0 };
	Color Yello = { 0,1,1 ,0 };
	Color Purple = { 1,0,1 ,0 };
	Color Cyan = { 0, 1, 1,0 };
	Color Black = { 0,0,0 ,0 };
	Color White = { 1,1,1 ,0 };
	Cube Cube1 = { {0,0,0,Red},{1,0,0,Green},{1,1,0,Red},{0,1,0,Blue},
	{0,0,1,Yello},{1,0,1,Purple},{1,1,1,Cyan},{1,0,1,Black} };
	Point cameraPlace = { 0,0,0,{0 ,0 ,0} };//在原点看
	Point cameraTarget = { 0,0,1,{0,0,0} };
	std::vector<double> up = { 0,1,0 };
	Matrix Xiw = constructXiwMatrix(cameraPlace, cameraTarget, up);
	//初始化image
	Color** image = new Color * [width];
	for (int i = 0; i < width; ++i) {
		image[i] = new Color[height];//用new运算符分配了一个大小为 height 的 Color 数组，并将其赋值给 image[i]
		memset(image[i], 0, height * sizeof(Color));//动态分配内存
		for (int j = 0; j < height; j++) {
			image[i][j].d = std::numeric_limits<double>::max();
		}
	}

	Color** image_buffer = new Color * [width*2];
	for (int i = 0; i < width*2; ++i) {
		image[i] = new Color[height*2];
		memset(image[i], 0, height*2 * sizeof(Color));//动态分配内存
		for (int j = 0; j < height; j++) {
			image[i][j].d = std::numeric_limits<double>::max();
		}
	}
	double** zBuffer = new double* [width];
	for (int i = 0; i < width; ++i) {
		zBuffer[i] = new double[height];
		memset(zBuffer[i], 0, height * sizeof(double));
		for (int j = 0; j < height; j++) {
			zBuffer[i][j] = depth;
		}
	}
	// 创建一个统一变换矩阵对象
	Matrix transformMatrix(4, 4);
	// 旋转变换
	//transformMatrix.Rotate(0.5f, 1.0f, 0.5f, 1.0f);
	transformMatrix = Rotate(0.3f, 1.0f, 1.0f, 1.0f, transformMatrix);
	// 缩放变换
	transformMatrix = Scale(200.0f, 200.0f, 200.0f, transformMatrix);
	// 平移变换
	transformMatrix = Translate(300, 300, 300, transformMatrix);
	// 获取内部矩阵
	Matrix perspectiveMatrix = perspectiveTransform(0.5);//image到perspective
	Matrix screenMatrix = screenTransform(width, height, 0.05);//perspective到screen
	//transformMatrix = perspectiveTransform(5, transformMatrix);
	//transformMatrix = screenTransform(width, height, 1, transformMatrix);
	transformMatrix = transformMatrix;
	Triangle Points2 = { {50,100,0,Red},{100,300,0,Green},{200,100,0,Blue} };
	//drawPlane(Points3, image, transformMatrix);
	Triangle Points4 = { Cube1.point0, Cube1.point7, Cube1.point3 };
	//drawPlane(Points4, image, transformMatrix);
	

	//drawCube(Cube1, image, transformMatrix);
	drawTriangle(Points2, image, image_buffer);

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
/*class TransformationMatrix {
private:
	Matrix matrix; // 统一变换矩阵

public:
	// 构造函数
	TransformationMatrix() {
		matrix = Matrix(4, 4);
	}

	// 获取内部矩阵
	const Matrix& GetMatrix() const {
		return matrix;
	}

	// 矩阵乘法
	TransformationMatrix operator*(const TransformationMatrix& other) const {
		TransformationMatrix result;
		result.matrix = matrix * other.matrix;
		return result;
	}

	// 平移变换
	Matrix Translate(float x, float y, float z,Matrix matrix) {
		Matrix translateMatrix(4, 4);
		translateMatrix(0, 3) = x;
		translateMatrix(1, 3) = y;
		translateMatrix(2, 3) = z;

		matrix =   matrix * translateMatrix;
		return matrix;
	}

	// 旋转变换
	Matrix Rotate(float angle, float x, float y, float z, Matrix matrix) {
		float cosTheta = cos(angle);
		float sinTheta = sin(angle);

		// 归一
		float length = sqrt(x * x + y * y + z * z);
		if (length != 0.0f) {
			x /= length;
			y /= length;
			z /= length;
		}

		Matrix rotateMatrix(4, 4);
		rotateMatrix(0, 0) = x * x + (1 - x * x) * cosTheta;
		rotateMatrix(0, 1) = x * y * (1 - cosTheta) - z * sinTheta;
		rotateMatrix(0, 2) = x * z * (1 - cosTheta) + y * sinTheta;

		rotateMatrix(1, 0) = x * y * (1 - cosTheta) + z * sinTheta;
		rotateMatrix(1, 1) = y * y + (1 - y * y) * cosTheta;
		rotateMatrix(1, 2) = y * z * (1 - cosTheta) - x * sinTheta;

		rotateMatrix(2, 0) = x * z * (1 - cosTheta) - y * sinTheta;
		rotateMatrix(2, 1) = y * z * (1 - cosTheta) + x * sinTheta;
		rotateMatrix(2, 2) = z * z + (1 - z * z) * cosTheta;

		matrix = matrix * rotateMatrix;
		return matrix;
	}

	// 缩放变换
	Matrix Scale(float scaleX, float scaleY, float scaleZ, Matrix matrix) {
		Matrix scaleMatrix(4, 4);
		scaleMatrix(0, 0) = scaleX;
		scaleMatrix(1, 1) = scaleY;
		scaleMatrix(2, 2) = scaleZ;
		scaleMatrix(3, 3) = 1.0;
		matrix = matrix * scaleMatrix;
		return matrix;
	}

};
*/
/*double reservePointZ(Triangle a, double x, double y) {
	double z = (-1 * (a.D + a.A * x + a.B * y)) / a.C;//根据平面函数的表达式解出z深度
	return z;
}*/
//plane projectionPlane = { 1,1,1,1 };
/*Vector3 projectionPoint(plane plane1, Vector3 point1) {
	float a = point1.x;
	float b = point1.y;
	float c = point1.z;
	float A = plane1.A;
	float B = plane1.B;
	float C = plane1.C;
	float D = plane1.D;
	float x, y, z;
	float m = (A * A + B * B + C * C);
	x = (-1 * A * (B * b + C * c + D)) + a * ((B * B) + (C * C)) / (m);
	y = (-1 * B * (A * a + C * c + D)) + b * ((A * A) + (C * C)) / (m);
	z = (-1 * C * (A * a + B * b + D)) + c * ((A * A) + (B * B)) / (m);
	return { x,y,z };
}*/
/*void projection(plane plane1, Color Color1, Color** image) {//投影到plane1平面呈现到image
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			Vector3 point = { x,y,1 };
			if (inPlane(plane1, point)) {
				image[x][y] = { (unsigned char)(Color1.r * (float)(z / depth)),
					(unsigned char)(Color1.g * (float)(z / depth)) ,
					(unsigned char)(Color1.b * (float)(z / depth)) };
			}
		}
	}
}*/
/*int inPlane(plane plane1, Point pointPlace) {
	if (inTriangle(plane1.A.x, plane1.A.y, plane1.B.x, plane1.B.y,
		plane1.C.x, plane1.C.y, pointPlace.x, pointPlace.y) ||
		inTriangle(plane1.B.x, plane1.B.y, plane1.C.x, plane1.C.y,
			plane1.D.x, plane1.D.y, pointPlace.x, pointPlace.y)) {
		return pointPlace.z;
	}
	else
		return false;
}*/
/*Vector2 removeZ(Vector3 a) {
	Vector2 b = { a.x,a.y };
	return b;
Color colorArray[12];
for (unsigned char i = 0; i < 12; i++) {
	colorArray[i] = { i*20,i*20,i*20 ,0};
}
}*/