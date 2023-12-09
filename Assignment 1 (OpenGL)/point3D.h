#ifndef POINT3D_H
#define POINT3D_H

#include <tuple>

#include "vector.h"

struct Point3D {
 public:
  double x, y, z;
  Point3D(double x = 0, double y = 0, double z = 0);
  Point3D operator+(const Vector& v) const;
  Point3D operator+=(const Vector& v);
  Point3D operator-(const Vector& v) const;
  Point3D operator-=(const Vector& v);
  Point3D operator*(const double& d) const;
  double distance(const Point3D& p) const;
  // returns a tuple of (a, b, c, d) where ax + by + cz + d = 0 is the plane
  friend std::tuple<double, double, double, double> get_plane_equation(
      const Point3D& p1, const Point3D& p2, const Point3D& p3);
};

#endif