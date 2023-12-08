#include "ball.h"

#include <cmath>

Ball::Ball(double radius, int sector_count, int stack_count, Vector dir)
    : radius(radius),
      sector_count(sector_count),
      stack_count(stack_count),
      dir(dir) {
  compute_vertices();
}

void Ball::update_radius(double radius) {
  if (radius == this->radius) return;
  this->radius = radius;
  compute_vertices();
}

void Ball::update_sector_count(int sector_count) {
  if (sector_count == this->sector_count) return;
  this->sector_count = sector_count;
  compute_vertices();
}

void Ball::update_stack_count(int stack_count) {
  if (stack_count == this->stack_count) return;
  this->stack_count = stack_count;
  compute_vertices();
}

void Ball::compute_vertices() {
  double stack_step = M_PI / stack_count;
  double sector_step = 2 * M_PI / sector_count;
  for (int i = 0; i <= stack_count; i++) {
    double stack_angle = M_PI / 2.0 - i * stack_step;  // range pi/2 to -pi/2
    for (int j = 0; j <= sector_count; j++) {
      double sector_angle = j * sector_step;  // range 0 to 2pi

      // printf("Stack Angle: %lf, Sector Angle: %lf\n", stack_angle * 180 /
      // M_PI,
      //        sector_angle * 180 / M_PI);

      // Vertex Position
      double x = radius * cos(stack_angle) * cos(sector_angle);
      double y = radius * cos(stack_angle) * sin(sector_angle);
      double z = radius * sin(stack_angle);

      vertices.push_back(Point3D(x, y, z));
      // printf("Vertex %d %d: (%lf, %lf, %lf)\n", i, j, x, y, z);
    }
  }
}