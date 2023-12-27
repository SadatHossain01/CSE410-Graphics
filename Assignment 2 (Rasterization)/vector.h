#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>

struct Vector {
 public:
  double x, y, z;
  Vector(double x = 0, double y = 0, double z = 0);
  Vector operator+(const Vector& v) const;
  Vector operator+=(const Vector& v);
  Vector operator-(const Vector& v) const;
  Vector operator-=(const Vector& v);
  Vector operator*(const double& d) const;
  friend Vector operator*(const double& d, const Vector& v);
  Vector operator*=(const double& d);
  Vector operator/(const double& d) const;
  Vector operator/=(const double& d);
  double dot(const Vector& v) const;
  Vector cross(const Vector& v) const;
  Vector normalize() const;
  Vector rotate(const Vector& axis, double angle) const;  // angle in degrees
  // normal should be normalized
  Vector get_reflection(const Vector& normal) const;
  bool check_normalized() const;
  bool check_orthogonal(const Vector& v) const;
  double norm() const;

  friend std::istream& operator>>(std::istream& is, Vector& v);
  friend std::ostream& operator<<(std::ostream& os, const Vector& v);
};

#endif