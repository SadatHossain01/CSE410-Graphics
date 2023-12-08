#ifndef BALL_H
#define BALL_H
#endif

#include <vector>

#include "point3D.h"
#include "vector.h"

struct Ball {
 public:
  double radius;
  int sector_count;
  int stack_count;
  Vector dir;
  std::vector<Point3D> vertices;

  Ball(double radius = 5, int sector_count = 25, int stack_count = 25,
       Vector dir = Vector(0, 0, 1));
  void update_radius(double radius);
  void update_sector_count(int sector_count);
  void update_stack_count(int stack_count);
  void compute_vertices();
};