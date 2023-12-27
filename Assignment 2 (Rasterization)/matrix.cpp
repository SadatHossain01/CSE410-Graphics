#include "matrix.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>

const double EPS = 1e-8;

Matrix::Matrix(int row, int col) {
  this->row = row;
  this->col = col;
  this->data =
      std::vector<std::vector<double>>(row, std::vector<double>(col, 0));
}

Matrix::Matrix(std::vector<std::vector<double>> data) {
  this->row = data.size();
  this->col = data[0].size();
  this->data = data;
}

Matrix Matrix::operator+(Matrix &other) const {
  if (this->row != other.row || this->col != other.col)
    throw std::invalid_argument("Matrix dimensions must match");
  Matrix result(this->row, this->col);
  for (int i = 0; i < this->row; i++) {
    for (int j = 0; j < this->col; j++) {
      result.data[i][j] = this->data[i][j] + other.data[i][j];
    }
  }
  return result;
}

Matrix Matrix::operator+=(Matrix &other) {
  if (this->row != other.row || this->col != other.col)
    throw std::invalid_argument("Matrix dimensions must match");
  *this = *this + other;
  return *this;
}

Matrix Matrix::operator-(Matrix &other) const {
  if (this->row != other.row || this->col != other.col)
    throw std::invalid_argument("Matrix dimensions must match");
  Matrix result(this->row, this->col);
  for (int i = 0; i < this->row; i++) {
    for (int j = 0; j < this->col; j++) {
      result.data[i][j] = this->data[i][j] - other.data[i][j];
    }
  }
  return result;
}

Matrix Matrix::operator-=(Matrix &other) {
  if (this->row != other.row || this->col != other.col)
    throw std::invalid_argument("Matrix dimensions must match");
  *this = *this - other;
  return *this;
}

Matrix Matrix::operator*(Matrix &other) const {
  if (this->col != other.row)
    throw std::invalid_argument(
        "First matrix's column must match second matrix's row");
  Matrix result(this->row, other.col);
  for (int k = 0; k < this->col; k++) {
    for (int i = 0; i < this->row; i++) {
      for (int j = 0; j < other.col; j++) {
        result.data[i][j] += this->data[i][k] * other.data[k][j];
      }
    }
  }
  return result;
}

Matrix Matrix::operator*=(Matrix &other) {
  if (this->col != other.row)
    throw std::invalid_argument(
        "First matrix's column must match second matrix's row");
  *this = *this * other;
  return *this;
}

Matrix Matrix::operator*(double scalar) const {
  Matrix result(this->row, this->col);
  for (int i = 0; i < this->row; i++) {
    for (int j = 0; j < this->col; j++) {
      result.data[i][j] = this->data[i][j] * scalar;
    }
  }
  return result;
}

Matrix Matrix::operator*=(double scalar) {
  *this = *this * scalar;
  return *this;
}

Matrix Matrix::operator/(double scalar) const {
  if (std::fabs(scalar) < EPS)
    throw std::invalid_argument("Scalar must not be zero");
  Matrix result(this->row, this->col);
  for (int i = 0; i < this->row; i++) {
    for (int j = 0; j < this->col; j++) {
      result.data[i][j] = this->data[i][j] / scalar;
    }
  }
  return result;
}

Matrix Matrix::operator/=(double scalar) {
  if (std::fabs(scalar) < EPS)
    throw std::invalid_argument("Scalar must not be zero");
  *this = *this / scalar;
  return *this;
}

Matrix Matrix::identity(int size) {
  Matrix result(size, size);
  for (int i = 0; i < size; i++) result.data[i][i] = 1;
  return result;
}

Matrix Matrix::transpose() const {
  Matrix result(this->col, this->row);
  for (int i = 0; i < this->col; i++) {
    for (int j = 0; j < this->row; j++) {
      result.data[i][j] = this->data[j][i];
    }
  }
  return result;
}

double Matrix::determinant() const {
  if (this->row != this->col)
    throw std::invalid_argument("Matrix must be square");
  if (this->row == 1) return this->data[0][0];
  double result = 0;
  for (int i = 0; i < this->col; i++) {
    Matrix temp(this->row - 1, this->col - 1);
    for (int j = 1; j < this->row; j++) {
      for (int k = 0; k < this->col; k++) {
        if (k < i)
          temp.data[j - 1][k] = this->data[j][k];
        else if (k > i)
          temp.data[j - 1][k - 1] = this->data[j][k];
      }
    }
    result += this->data[0][i] * temp.determinant() * (i % 2 == 0 ? 1 : -1);
  }
  return result;
}

Matrix Matrix::inverse() const {
  if (this->row != this->col)
    throw std::invalid_argument("Matrix must be square");
  double det = this->determinant();
  if (std::fabs(det) < EPS)
    throw std::invalid_argument("Matrix must be invertible");
  Matrix result(this->row, this->col);
  for (int i = 0; i < this->row; i++) {
    for (int j = 0; j < this->col; j++) {
      Matrix temp(this->row - 1, this->col - 1);
      for (int k = 0; k < this->row; k++) {
        for (int l = 0; l < this->col; l++) {
          if (k < i && l < j)
            temp.data[k][l] = this->data[k][l];
          else if (k < i && l > j)
            temp.data[k][l - 1] = this->data[k][l];
          else if (k > i && l < j)
            temp.data[k - 1][l] = this->data[k][l];
          else if (k > i && l > j)
            temp.data[k - 1][l - 1] = this->data[k][l];
        }
      }
      result.data[i][j] = temp.determinant() * (i + j % 2 == 0 ? 1 : -1) / det;
    }
  }
  return result.transpose();
}

std::ostream &operator<<(std::ostream &os, const Matrix &matrix) {
  for (int i = 0; i < matrix.row; i++) {
    for (int j = 0; j < matrix.col; j++) os << matrix.data[i][j] << " ";
    if (i != matrix.row - 1) os << std::endl;
  }
  return os;
}