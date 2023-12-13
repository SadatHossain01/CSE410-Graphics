#ifndef BALL_H
#define BALL_H
#endif

#include <vector>

#include "point3D.h"
#include "vector.h"

struct Ball {
 public:
  Point3D center;
  Vector dir, right, up;
  const double radius;
  const int sector_count;
  const int stack_count;
  const double dir_scalar_multiplier = 1;
  const double dt = 20;           // ms
  double dir_rotation_angle = 1;  // in degrees
  std::vector<Point3D> ball_vertices;
  std::vector<Point3D> box_vertices;

  Ball(double radius = 5, int sector_count = 25, int stack_count = 25);
  void compute_vertices();
  void go_forward();
  void go_backward();
  void rotate_dir_ccw();
  void rotate_dir_cw();
  void rotate_ball_vertices(const Vector& axis, double angle);

  // Collision Related Functions
  bool is_ball_inside_box(Point3D c, double rad,
                          const std::vector<Point3D>& box_vertices);
  double get_distance_from_wall(int wall_idx);
  std::vector<std::pair<int, double>>
  get_collision_distances();  // returns vector of <wall_index, distance>
  int will_collision_occur();
  int get_facing_wall_idx();  // returns which wall the ball is going to hit if
                              // uninterrupted
  double next_collision_time();
};