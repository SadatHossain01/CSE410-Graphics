#ifndef LINE_H
#define LINE_H
#endif

#include "point3D.h"
#include "vector.h"

struct Line {
 public:
  Point3D p0, p1;
  Vector dir;
  Line(Point3D p0, Point3D p1);
  Line(Point3D p0, Vector dir);
  std::pair<bool, Point3D> get_intersection_point(const Line& l);
  double get_distance_from_point(const Point3D& p);
  double get_distance_from_point_along_vector(const Point3D& p,
                                              const Vector& v);
};