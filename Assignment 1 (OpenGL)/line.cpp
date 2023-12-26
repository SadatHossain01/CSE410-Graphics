#include "line.h"

#include <cmath>

Line::Line(Point3D p0, Point3D p1) : p0(p0), p1(p1) {
  dir = Vector(p1.x - p0.x, p1.y - p0.y, p1.z - p0.z);
}

Line::Line(Point3D p0, Vector dir) : p0(p0), dir(dir) { p1 = p0 + dir; }

std::pair<bool, Point3D> Line::get_intersection_point(const Line& l) {
  /*
    p0.x + t * dir.x = l.p0.x + s * l.dir.x
    p0.y + t * dir.y = l.p0.y + s * l.dir.y
    p0.z + t * dir.z = l.p0.z + s * l.dir.z
    We can solve for t and s using Cramer's rule, considering the first two
    equations
  */
  /*
    a1 * t + b1 * s + c1 = 0
    a2 * t + b2 * s + c2 = 0
  */
  double a1, b1, c1, a2, b2, c2;
  std::tie(a1, b1, c1) = std::make_tuple(dir.x, -l.dir.x, p0.x - l.p0.x);
  std::tie(a2, b2, c2) = std::make_tuple(dir.y, -l.dir.y, p0.y - l.p0.y);

  double det = a1 * b2 - a2 * b1;
  if (fabs(det) < 1e-6) return std::make_pair(false, Point3D());

  double t = (b1 * c2 - b2 * c1) / det;
  double s = (a2 * c1 - a1 * c2) / det;

  Point3D intersection_point = p0 + Point3D(dir.x * t, dir.y * t, dir.z * t);
  return std::make_pair(true, intersection_point);
}

// https://youtu.be/Uf_nDXzs2EA
double Line::get_distance_from_point(const Point3D& p) {
  Vector b = Vector(p.x - p0.x, p.y - p0.y, p.z - p0.z);
  Vector c = dir.cross(b);
  return c.norm() / dir.norm();
}

double Line::get_distance_from_point_along_vector(const Point3D& p,
                                                  const Vector& v) {
  Line other = Line(p, v);
  std::pair<bool, Point3D> intersection_point = get_intersection_point(other);
  if (intersection_point.first) {
    return intersection_point.second.distance(p);
  } else {
    // now the lines are parallel
    return get_distance_from_point(p);
  }
}