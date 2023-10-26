#include "Transfrom.h"

Matrix Transfrom::Translate(double x, double y, double z, Matrix matrix)
{
	Matrix translateMatrix(4, 4);
	translateMatrix(0, 3) = x;
	translateMatrix(1, 3) = y;
	translateMatrix(2, 3) = z;
	matrix = translateMatrix * matrix;
	return matrix;
}
Matrix Transfrom::Rotate(double angle, double x, double y, double z, Matrix matrix)
{
	double cosTheta = cos(angle);
	double sinTheta = sin(angle);

	// น้าป
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
