#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <ctime>

const int width = 800;
const int height = 600;
const int ssaaFactor = 2; // SSAA采样因子
int ssaaWidth = width * ssaaFactor;
int ssaaHeight = height * ssaaFactor;
struct Color {//定义一个Color结构体RGB三元素
	unsigned char r, g, b;
};
Color add(Color a, Color b, Color c, Color d) {
	Color e = { 0,0,0 };
	e.r = (a.r + b.r + c.r + d.r) / 4;
	e.g = (a.g + b.g + c.g + d.g) / 4;
	e.b = (a.b + b.b + c.b + d.b) / 4;
	return e;
}
int getRandomInt(int min, int max) {
	//srand(time(0)); // 设置随机数种子
	int randomInt = min + (rand() % (max - min + 1)); // 生成[min, max]范围内的随机整数
	return randomInt;
}
float getRandomFloat() {
	return static_cast<float>(rand()) / RAND_MAX; // 生成0到1之间的随机浮点数
}

int cross(int a, int b, int c, int d) {
	return a * d - b * c;
}
int inTrigger(int x0, int y0, int x1, int y1, int x2, int y2, int x, int y) {
	int c0 = x - x0;
	int d0 = y - y0;
	int c1 = x - x1;
	int d1 = y - y1;
	int c2 = x - x2;
	int d2 = y - y2;
	int a0 = x0 - x1;
	int b0 = y0 - y1;
	int a1 = x1 - x2;
	int b1 = y1 - y2;
	int a2 = x2 - x0;
	int b2 = y2 - y0;
	int m0 = cross(a0, b0, c0, d0);
	int m1 = cross(a1, b1, c1, d1);
	int m2 = cross(a2, b2, c2, d2);
	if (m0 < 0 && m1 < 0 && m2 < 0 || m0>0 && m1>0 && m2>0) {
		return 1;
	}
	else
		return 0;
}

void drawSsaaTrigger(int x0, int y0, int x1, int y1, int x2, int y2,
	Color color0, Color** image) {
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			for (int sy = 0; sy < ssaaFactor; ++sy) {
				for (int sx = 0; sx < ssaaFactor; ++sx) {
					if (inTrigger(ssaaFactor * x0, ssaaFactor * y0, ssaaFactor * x1, ssaaFactor * y1,
						ssaaFactor * x2, ssaaFactor * y2, ssaaFactor * x + sx, ssaaFactor * y + sy)) {
						image[ssaaFactor * x + sx][ssaaFactor * y + sy] = color0;
					}
				}
			}
		}
	}
}
void drawtrigger(int x0, int y0, int x1, int y1, int x2, int y2,
	Color color0, Color** image) {
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			if (inTrigger(x0, y0, x1, y1, x2, y2, x, y)) {
				image[x][y] = color0;
			}
		}
	}
}
void saveImage1(const char* filename1, Color** image) {
	FILE* fp = freopen(filename1, "w+", stdout);
	std::cout << "P6\n" << width << " " << height << "\n255\n";//声明格式
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			std::cout << image[x][y].r << image[x][y].g << image[x][y].b;
		}
	}

	fclose(fp);
}
void save_ssaaImage1(const char* filename1, Color** image, Color** ssaaImage) {
	FILE* fp = freopen(filename1, "w+", stdout);
	std::cout << "P6\n" << width << " " << height << "\n255\n";//声明格式
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			image[x][y] = add(ssaaImage[x * ssaaFactor][y * ssaaFactor],
				ssaaImage[x * ssaaFactor + 1][y * ssaaFactor], ssaaImage[x * ssaaFactor][y * ssaaFactor + 1]
				, ssaaImage[x * ssaaFactor + 1][y * ssaaFactor + 1]);
			std::cout << image[x][y].r << image[x][y].g << image[x][y].b;
		}
	}
}
int main() {
	// 创建一个空白的图像
	Color** image = new Color * [width];
	for (int i = 0; i < width; ++i) {
		image[i] = new Color[height];//用new运算符分配了一个大小为 height 的 Color 数组，并将其赋值给 image[i]
		memset(image[i], 0, height * sizeof(Color));//动态分配内存
	}
	// 创建SSAA图像

	Color** ssaaImage = new Color * [ssaaWidth];
	for (int i = 0; i < ssaaWidth; ++i) {
		ssaaImage[i] = new Color[ssaaHeight];
		memset(ssaaImage[i], 0, ssaaHeight * sizeof(Color));
	}


	// 绘制图像
	int x0 = 100, y0 = 100;
	int x1 = 700, y1 = 300;
	int x2 = 0, y2 = 500;
	Color Green = { 255, 0, 0 };  // 设置线段颜色为绿色
	Color Blue = { 0, 255, 0 };
	Color Red = { 0,0,255 };
	//drawLine(x0, y0, x1, y1, lineColor, image);
	drawtrigger(x0, y0, x1, y1, x2, y2, Green, image);
	drawSsaaTrigger(x0, y0, x1, y1, x2, y2, Green, ssaaImage);

	const char* filename1 = "output1.ppm";
	saveImage1(filename1, image);
	const char* ssaaFilename = "output_ssaa.ppm";
	save_ssaaImage1(ssaaFilename, image, ssaaImage);

	// 释放内存
	for (int i = 0; i < width; ++i) {
		delete[] image[i];
	}
	delete[] image;

	for (int i = 0; i < ssaaWidth; ++i) {
		delete[] ssaaImage[i];
	}
	delete[] ssaaImage;

	return 0;


}
/*float num1, num2, num3;
	for (int i = 0; i < 100; i++) {
		x0 = getRandomInt(0, 800);
		x1 = getRandomInt(0, 800);
		x2 = getRandomInt(0, 800);
		y0 = getRandomInt(0, 600);
		y1 = getRandomInt(0, 600);
		y2 = getRandomInt(0, 600);
		num1 = getRandomFloat();
		num2 = getRandomFloat();
		num3 = getRandomFloat();
		Color Color1 = { (int)255 * num1,(int)255 * num2,(int)255 * num3 };
		drawSSAAtrigger(x0, y0, x1, y1, x2, y2, Color1, ssaaImage);//画出2*2放大的图像
	}*/
	/*void drawLine(int x0, int y0, int x1, int y1, Color color, Color** image) {//画线函数
		float slope = static_cast<float>(y1 - y0) / static_cast<float>(x1 - x0);//斜率
		if (std::abs(slope) <= 1) {//斜率绝对值小于等于1时不改变坐标值
			if (x0 > x1) {
				std::swap(x0, x1);
				std::swap(y0, y1);
			}
			for (int x = x0; x <= x1; ++x) {//画线
				int y = y0 + static_cast<int>((x - x0) * slope);
				image[x][y] = color;//对image的像素坐标上色
			}
		}
		else {
			if (y0 > y1) {
				std::swap(x0, x1);//相当于翻转坐标轴
				std::swap(y0, y1);
			}
			slope = 1 / slope;
			for (int y = y0; y <= y1; ++y) {
				int x = x0 + static_cast<int>((y - y0) * slope);
				image[x][y] = color;
			}
		}
	}*///
	/*/*void saveImage(const std::string& filename, Color** image) {//保存图像
		std::ofstream file(filename, std::ios::out | std::ios::binary);
		file << "P6\n";//ppm格式
		file << width << " " << height << "\n";//
		file << "255\n";//
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				file << image[x][y].r << image[x][y].g << image[x][y].b;
			}
		}
		file.close();
		std::cout << "Image saved to " << filename << std::endl;
	}
	*/