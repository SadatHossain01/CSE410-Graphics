#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <iostream>
#include <vector>

#include "matrix.h"
#include "vector.h"

class Triangle {
 public:
  std::vector<Matrix> vertices;  // Each triangle is a 4x1 matrix

  Triangle();
  Triangle(const Vector& v1, const Vector& v2, const Vector& v3);
  friend std::istream& operator>>(std::istream& is, Triangle& t);
  friend std::ostream& operator<<(std::ostream& os, const Triangle& t);
  void transform(const Matrix& m);
};

#endif