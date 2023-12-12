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

// returns a tuple of (a, b, c, d) where ax + by + cz + d = 0 is the plane
std::tuple<double, double, double, double> get_plane_equation(
    const Point3D& p1, const Point3D& p2, const Point3D& p3) {
  double a, b, c, d;
  Vector v1(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);
  Vector v2(p3.x - p1.x, p3.y - p1.y, p3.z - p1.z);
  Vector normal = v1.cross(v2);
  a = normal.x;
  b = normal.y;
  c = normal.z;
  d = -(a * p1.x + b * p1.y + c * p1.z);
  return std::make_tuple(a, b, c, d);
}