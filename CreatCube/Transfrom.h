#pragma once
#include "matrix.h"
class Transfrom
{
public:
	Matrix Translate(double x, double y, double z, Matrix matrix);
	Matrix Rotate(double angle, double x, double y, double z, Matrix matrix);

};

