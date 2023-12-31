#include "line.h"

#include <cmath>

Line::Line(Point3D p0, Point3D p1) : p0(p0), p1(p1) {
  dir = Vector(p1.x - p0.x, p1.y - p0.y, p1.z - p0.z);
}

Line::Line(Point3D p0, Vector dir) : p0(p0), dir(dir) { p1 = p0 + dir; }

// https://www.songho.ca/math/line/line.html
std::pair<bool, Point3D> Line::get_intersection_point(const Line& l) {
  Vector vec = dir.cross(l.dir);  // this->dir x l.dir

  double res = vec.dot(vec);  // |vec|^2
  if (fabs(res) < 1e-6) {
    // |this->dir x l.dir| = 0, so this->dir and l.dir are parallel
    return std::make_pair(false, Point3D());
  }

  Vector w = Vector(l.p0.x - p0.x, l.p0.y - p0.y, l.p0.z - p0.z);
  Vector u = w.cross(l.dir);
  double t = u.dot(vec) / res;

  Point3D p = Point3D(p0.x + t * dir.x, p0.y + t * dir.y, p0.z + t * dir.z);
  return std::make_pair(true, p);
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