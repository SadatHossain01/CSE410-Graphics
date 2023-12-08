#include "point3D.h"

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