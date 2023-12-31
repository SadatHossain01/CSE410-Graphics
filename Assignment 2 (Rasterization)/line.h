#ifndef LINE_H
#define LINE_H
#endif

#include "vector.h"

class Point {
 public:
  double x, y, z;
  Point(double x = 0, double y = 0, double z = 0);
  friend std::ostream& operator<<(std::ostream& os, const Point& p);
  double distance(const Point& p);
};

class Line {
 public:
  Point p0, p1;
  Vector dir;
  Line(Point p0, Point p1);
  Line(Point p0, Vector dir);
  std::pair<bool, Point> get_intersection_point(const Line& l,
                                                bool debug_mode = false) const;
  double get_distance_from_point(const Point& p) const;
  double get_distance_from_point_along_vector(const Point& p,
                                              const Vector& v) const;
  double get_distance_from_line(const Line& l) const;
  friend std::ostream& operator<<(std::ostream& os, const Line& l);
};