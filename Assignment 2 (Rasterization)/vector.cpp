#include "vector.h"

#include <cassert>
#include <cmath>
#include <limits>
#include <stdexcept>

const double PI = 2 * acos(0.0);

Vector::Vector(double x, double y, double z) : x(x), y(y), z(z) {}

Vector Vector::operator+(const Vector& v) const {
  return Vector(x + v.x, y + v.y, z + v.z);
}

Vector Vector::operator+=(const Vector& v) {
  x += v.x, y += v.y, z += v.z;
  return *this;
}

Vector Vector::operator-(const Vector& v) const {
  return Vector(x - v.x, y - v.y, z - v.z);
}

Vector Vector::operator-=(const Vector& v) {
  x -= v.x, y -= v.y, z -= v.z;
  return *this;
}

Vector Vector::operator*(const double& d) const {
  return Vector(x * d, y * d, z * d);
}

Vector operator*(const double& d, const Vector& v) {
  return Vector(v.x * d, v.y * d, v.z * d);
}

Vector Vector::operator*=(const double& d) {
  x *= d, y *= d, z *= d;
  return *this;
}

Vector Vector::operator/(const double& d) const {
  if (fabs(d) <= std::numeric_limits<double>::epsilon())
    throw std::invalid_argument("Division by zero");
  return Vector(x / d, y / d, z / d);
}

Vector Vector::operator/=(const double& d) {
  if (fabs(d) <= std::numeric_limits<double>::epsilon())
    throw std::invalid_argument("Division by zero");
  x /= d, y /= d, z /= d;
  return *this;
}

double Vector::dot(const Vector& v) const {
  return x * v.x + y * v.y + z * v.z;
}

Vector Vector::cross(const Vector& v) const {
  return Vector(y * v.z - v.y * z, v.x * z - x * v.z, x * v.y - v.x * y);
}

Vector Vector::normalize() const {
  double length = sqrt(x * x + y * y + z * z);
  if (fabs(length) <= std::numeric_limits<double>::epsilon())
    throw std::invalid_argument("Vector magnitude is 0");
  return Vector(x / length, y / length, z / length);
}

Vector Vector::rotate(const Vector& axis, double angle) const {
  // Rodrigues' Rotation Formula
  // https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
  double theta = angle * PI / 180;
  Vector k = axis.normalize();
  Vector v1 = *this * cos(theta);
  Vector v2 = k.cross(*this) * sin(theta);
  Vector v3 = k * k.dot(*this) * (1 - cos(theta));
  return v1 + v2 + v3;
}

Vector Vector::get_reflection(const Vector& normal) const {
  assert(normal.check_normalized());
  Vector v = *this;
  Vector n = normal;
  return v - 2 * v.dot(n) * n;
}

bool Vector::check_normalized() const {
  double length = sqrt(x * x + y * y + z * z);
  return fabs(length - 1) <= std::numeric_limits<double>::epsilon();
}

bool Vector::check_orthogonal(const Vector& v) const {
  return fabs(this->dot(v)) <= std::numeric_limits<double>::epsilon();
}

double Vector::norm() const { return sqrt(x * x + y * y + z * z); }

std::istream& operator>>(std::istream& is, Vector& v) {
  is >> v.x >> v.y >> v.z;
  return is;
}

std::ostream& operator<<(std::ostream& os, const Vector& v) {
  // os << v.x << " " << v.y << " " << v.z;
  os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
  return os;
}