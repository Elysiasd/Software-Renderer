#include <iostream>
#include <vector>

class Matrix {
private:
    std::vector<std::vector<float>> data; // 二维浮点矩阵data
    int rows; // 行
    int cols; // 列

public:
    Matrix() : rows(0), cols(0) {}
    // 构造函数
    Matrix(int numRows, int numCols) : rows(numRows), cols(numCols)
    {
        data.resize(rows, std::vector<float>(cols, 0.0f));
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
    float& operator()(int row, int col)
    {
        return data[row][col];
    }

    // 获取矩阵中特定位置的元素（const版本）
    const float& operator()(int row, int col) const
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
                float entry = 0.0f;

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
class TransformationMatrix {
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
    void Translate(float x, float y, float z) {
        Matrix translateMatrix(4, 4);
        translateMatrix(0, 3) = x;
        translateMatrix(1, 3) = y;
        translateMatrix(2, 3) = z;

        matrix = matrix * translateMatrix;
    }

    // 旋转变换
    void Rotate(float angle, float x, float y, float z) {
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
        rotateMatrix(0, 0) = cosTheta + x * x * (1 - cosTheta);
        rotateMatrix(0, 1) = x * y * (1 - cosTheta) - z * sinTheta;
        rotateMatrix(0, 2) = x * z * (1 - cosTheta) + y * sinTheta;

        rotateMatrix(1, 0) = x * y * (1 - cosTheta) + z * sinTheta;
        rotateMatrix(1, 1) = cosTheta + y * y * (1 - cosTheta);
        rotateMatrix(1, 2) = y * z * (1 - cosTheta) - x * sinTheta;

        rotateMatrix(2, 0) = x * z * (1 - cosTheta) - y * sinTheta;
        rotateMatrix(2, 1) = y * z * (1 - cosTheta) + x * sinTheta;
        rotateMatrix(2, 2) = cosTheta + z * z * (1 - cosTheta);

        matrix = matrix * rotateMatrix;
    }

    // 缩放变换
    void Scale(float scaleX, float scaleY, float scaleZ) {
        Matrix scaleMatrix(4, 4);
        scaleMatrix(0, 0) = scaleX;
        scaleMatrix(1, 1) = scaleY;
        scaleMatrix(2, 2) = scaleZ;

        matrix = matrix * scaleMatrix;
    }
};

int main()
{
    // 创建一个统一变换矩阵对象
    TransformationMatrix transformMatrix;

    // 平移变换
    transformMatrix.Translate(1.0f, 2.0f, 3.0f);

    // 旋转变换
    transformMatrix.Rotate(0.5f, 0.0f, 1.0f, 0.0f);

    // 缩放变换
    transformMatrix.Scale(2.0f, 2.0f, 2.0f);

    // 获取内部矩阵
    const Matrix& finalMatrix = transformMatrix.GetMatrix();

    for (int i = 0; i < finalMatrix.GetRows(); i++)
    {
        for (int j = 0; j < finalMatrix.GetCols(); j++)
        {
            std::cout << finalMatrix(i, j) << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}