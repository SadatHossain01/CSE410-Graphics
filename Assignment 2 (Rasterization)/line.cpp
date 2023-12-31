#include "line.h"

#include <cmath>
#include <limits>
#include <tuple>

Point::Point(double x, double y, double z) : x(x), y(y), z(z) {}

double Point::distance(const Point& p) {
  return sqrt(pow(x - p.x, 2) + pow(y - p.y, 2) + pow(z - p.z, 2));
}

std::ostream& operator<<(std::ostream& os, const Point& p) {
  os << "(" << p.x << ", " << p.y << ", " << p.z << ")";
  return os;
}

Line::Line(Point p0, Point p1) : p0(p0), p1(p1) {
  dir = Vector(p1.x - p0.x, p1.y - p0.y, p1.z - p0.z);
}

Line::Line(Point p0, Vector dir) : p0(p0), dir(dir) {
  p1 = Point(p0.x + dir.x, p0.y + dir.y, p0.z + dir.z);
}

// https://www.songho.ca/math/line/line.html
std::pair<bool, Point> Line::get_intersection_point(const Line& l,
                                                    bool debug_mode) const {
  Vector vec = dir.cross(l.dir);  // this->dir x l.dir

  double res = vec.dot(vec);  // |vec|^2
  if (fabs(res) <= std::numeric_limits<double>::epsilon()) {
    // |this->dir x l.dir| = 0, so this->dir and l.dir are parallel
    return std::make_pair(false, Point());
  }

  Vector w = Vector(l.p0.x - p0.x, l.p0.y - p0.y, l.p0.z - p0.z);
  Vector u = w.cross(l.dir);
  double t = u.dot(vec) / res;

  Point p = Point(p0.x + t * dir.x, p0.y + t * dir.y, p0.z + t * dir.z);
  return std::make_pair(true, p);
}

// https://youtu.be/Uf_nDXzs2EA
double Line::get_distance_from_point(const Point& p) const {
  Vector b = Vector(p.x - p0.x, p.y - p0.y, p.z - p0.z);
  Vector c = dir.cross(b);
  return c.norm() / dir.norm();
}

double Line::get_distance_from_point_along_vector(const Point& p,
                                                  const Vector& v) const {
  Line other = Line(p, v);
  std::pair<bool, Point> intersection_point = get_intersection_point(other);
  if (intersection_point.first) {
    return intersection_point.second.distance(p);
  } else {
    // now the lines are parallel
    return get_distance_from_point(p);
  }
}

double Line::get_distance_from_line(const Line& l) const {
  Vector cross = dir.cross(l.dir).normalize();
  Vector b = Vector(p0.x - l.p0.x, p0.y - l.p0.y, p0.z - l.p0.z);
  return fabs(b.dot(cross));
}

std::ostream& operator<<(std::ostream& os, const Line& l) {
  // os << "(" << l.p0.x << ", " << l.p0.y << ", " << l.p0.z << ") + t(" <<
  // l.dir.x
  //    << ", " << l.dir.y << ", " << l.dir.z << ")";
  os << l.p0 << " -> " << l.p1;
  return os;
}