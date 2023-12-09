#include "ball.h"

#include <cmath>
#include <iostream>

const double EPS = 1e-6;

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

bool Ball::does_collide_with(std::vector<Point3D> wall_vertices) {
  // Assumption: ball is in the xy plane
  // iterate over the walls of the room/closed space
  printf("Center: (%lf, %lf, %lf)\n", center.x, center.y, center.z);
  for (int i = 0; i < wall_vertices.size() - 1; i++) {
    Point3D a = wall_vertices[i];
    Point3D b = wall_vertices[i + 1];
    Point3D c =
        a +
        Vector(0, 0, 1);  // assuming the wall is perpendicular to the xy plane
    std::tuple<double, double, double, double> plane_equation =
        get_plane_equation(a, b, c);
    double a1 = std::get<0>(plane_equation), b1 = std::get<1>(plane_equation),
           c1 = std::get<2>(plane_equation), d1 = std::get<3>(plane_equation);
    double numerator = fabs(a1 * center.x + b1 * center.y + c1 * center.z + d1);
    double denominator = sqrt(a1 * a1 + b1 * b1 + c1 * c1);
    double distance = numerator / denominator;

    printf("Wall %d: (%lf, %lf, %lf) (%lf, %lf, %lf) (%lf, %lf, %lf)\n", i, a.x,
           a.y, a.z, b.x, b.y, b.z, c.x, c.y, c.z);
    printf("Distance: %lf\n", distance);
    if (fabs(distance - radius) < EPS) {
      // printf("Distance: %lf\n", distance);
      return true;
    }
  }
  return false;
}
