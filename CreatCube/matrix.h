#pragma once
#include <iostream>
#include <vector>
class Matrix {
private:
	std::vector<std::vector<double>> data; // ��ά�������data
	int rows; // ��
	int cols; // ��

public:
	Matrix() : rows(0), cols(0) {}
	// ���캯��
	Matrix(int numRows, int numCols) : rows(numRows), cols(numCols)
	{
		data.resize(rows, std::vector<double>(cols, 0.0f));
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				data[i][j] = (i == j) ? 1.0f : 0.0f;
			}
		}
	}
	// ��ȡ���������
	int GetRows() const
	{
		return rows;
	}

	// ��ȡ���������
	int GetCols() const
	{
		return cols;
	}

	// ��ȡ�������ض�λ�õ�Ԫ��
	double& operator()(int row, int col)
	{
		return data[row][col];
	}

	// ��ȡ�������ض�λ�õ�Ԫ�أ�const�汾��
	const double& operator()(int row, int col) const
	{
		return data[row][col];
	}

	// ����˷�
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
