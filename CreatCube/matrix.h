#pragma once
#include <iostream>
#include <vector>
class Matrix {
private:
	std::vector<std::vector<double>> data; // 二维浮点矩阵data
	int rows; // 行
	int cols; // 列

public:
	Matrix() : rows(0), cols(0) {}
	// 构造函数
	Matrix(int numRows, int numCols) : rows(numRows), cols(numCols)
	{
		data.resize(rows, std::vector<double>(cols, 0.0f));
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				data[i][j] = (i == j) ? 1.0f : 0.0f;
			}
		}
	}
	// 获取矩阵的行数
	int GetRows() const
	{
		return rows;
	}

	// 获取矩阵的列数
	int GetCols() const
	{
		return cols;
	}

	// 获取矩阵中特定位置的元素
	double& operator()(int row, int col)
	{
		return data[row][col];
	}

	// 获取矩阵中特定位置的元素（const版本）
	const double& operator()(int row, int col) const
	{
		return data[row][col];
	}

	// 矩阵乘法
	Matrix operator*(const Matrix& other) const
	{
		if (cols != other.rows)
		{
			throw std::runtime_error("Matrix dimensions are not compatible for multiplication");
		}

		Matrix result(rows, other.cols);

		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < other.cols; j++)
			{
				double entry = 0.0;

				for (int k = 0; k < cols; k++)
				{
					entry += data[i][k] * other(k, j);
				}

				result(i, j) = entry;
			}
		}

		return result;
	}
};
