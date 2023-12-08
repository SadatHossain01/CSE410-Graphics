#include "ball.h"

#include <cmath>
#include <iostream>

Ball::Ball(double radius, int sector_count, int stack_count)
    : radius(radius), sector_count(sector_count), stack_count(stack_count) {
  angular_speed = speed * 180 / (radius * M_PI);
  dir = Vector(1, 1, 0).normalize();
  right = Vector(1, -1, 0).normalize();
  up = right.cross(dir);
  center = Point3D(0, 0, 5);
  compute_vertices();
}

// vertices are computed here considering the origin (0, 0, 0) as the center of
// the ball
void Ball::compute_vertices() {
  double stack_step = M_PI / stack_count;
  double sector_step = 2 * M_PI / sector_count;
  for (int i = 0; i <= stack_count; i++) {
    double stack_angle = M_PI / 2.0 - i * stack_step;  // range pi/2 to -pi/2
    for (int j = 0; j <= sector_count; j++) {
      double sector_angle = j * sector_step;  // range 0 to 2pi

      // Vertex Position
      double x = radius * cos(stack_angle) * cos(sector_angle);
      double y = radius * cos(stack_angle) * sin(sector_angle);
      double z = radius * sin(stack_angle);

      vertices.push_back(Point3D(x, y, z));
      // printf("Vertex %d %d: (%lf, %lf, %lf)\n", i, j, x, y, z);
    }
  }
}

void Ball::go_forward() {
  center.x += speed * dir.x;
  center.y += speed * dir.y;
  center.z += speed * dir.z;
  rotation_angle += angular_speed;
}

void Ball::go_backward() {
  center.x -= speed * dir.x;
  center.y -= speed * dir.y;
  center.z -= speed * dir.z;
  rotation_angle -= angular_speed;
}

void Ball::rotate_dir_ccw() {
  dir = dir.rotate(up, angular_speed);
  right = right.rotate(up, angular_speed);
}

void Ball::rotate_dir_cw() {
  dir = dir.rotate(up, -angular_speed);
  right = right.rotate(up, -angular_speed);
}
