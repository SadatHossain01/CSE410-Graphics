#ifndef BALL_H
#define BALL_H
#endif

#include <vector>

#include "point3D.h"
#include "vector.h"

struct Ball {
 public:
  double rotation_angle = 0;
  const double radius;
  const int sector_count;
  const int stack_count;
  const double speed = 1;
  double angular_speed;
  Point3D center;
  Vector dir, right, up;
  std::vector<Point3D> vertices;

  Ball(double radius = 5, int sector_count = 25, int stack_count = 25);
  void compute_vertices();
  void go_forward();
  void go_backward();
  void rotate_dir_ccw();
  void rotate_dir_cw();
  bool does_collide_with(std::vector<Point3D> wall_vertices);
};