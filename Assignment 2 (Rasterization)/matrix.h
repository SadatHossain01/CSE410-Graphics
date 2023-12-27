#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <vector>

class Matrix {
 public:
  int row, col;
  std::vector<std::vector<double>> data;
  Matrix(int row, int col);
  Matrix(std::vector<std::vector<double>> data);
  Matrix operator+(Matrix &other) const;
  Matrix operator+=(Matrix &other);
  Matrix operator-(Matrix &other) const;
  Matrix operator-=(Matrix &other);
  Matrix operator*(Matrix &other) const;
  Matrix operator*=(Matrix &other);
  Matrix operator*(double scalar) const;
  Matrix operator*=(double scalar);
  Matrix operator/(double scalar) const;
  Matrix operator/=(double scalar);
  static Matrix identity(int size);
  Matrix transpose() const;
  double determinant() const;
  Matrix inverse() const;
  friend std::ostream &operator<<(std::ostream &os, const Matrix &m);
};

#endif