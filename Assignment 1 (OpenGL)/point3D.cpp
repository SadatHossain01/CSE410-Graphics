#include "point3D.h"

#include <cmath>

Point3D::Point3D(double x, double y, double z) : x(x), y(y), z(z) {}

Point3D Point3D::operator+(const Vector& v) const {
  return Point3D(x + v.x, y + v.y, z + v.z);
}

Point3D Point3D::operator+=(const Vector& v) {
  x += v.x;
  y += v.y;
  z += v.z;
  return *this;
}

Point3D Point3D::operator-(const Vector& v) const {
  return Point3D(x - v.x, y - v.y, z - v.z);
}

Point3D Point3D::operator-=(const Vector& v) {
  x -= v.x;
  y -= v.y;
  z -= v.z;
  return *this;
}

Point3D Point3D::operator*(const double& d) const {
  return Point3D(x * d, y * d, z * d);
}

double Point3D::distance(const Point3D& p) const {
  return sqrt(pow(x - p.x, 2) + pow(y - p.y, 2) + pow(z - p.z, 2));
}