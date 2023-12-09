#ifndef POINT3D_H
#define POINT3D_H

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
};

#endif