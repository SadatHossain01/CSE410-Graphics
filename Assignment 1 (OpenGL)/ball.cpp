#include "ball.h"

#include <algorithm>
#include <cmath>
#include <iostream>

const double EPS = 1e-6;
const double INF = 1e9;

Ball::Ball(double radius, int sector_count, int stack_count)
    : radius(radius), sector_count(sector_count), stack_count(stack_count) {
  dir_rotation_angle = dir_scalar_multiplier * 180 / (radius * M_PI);
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

      ball_vertices.push_back(Point3D(x, y, z));
      // printf("Vertex %d %d: (%lf, %lf, %lf)\n", i, j, x, y, z);
    }
  }
}

void Ball::go_forward() {
  double usual_dist_change = dir_scalar_multiplier * dir.norm();
  Point3D temp = center + dir_scalar_multiplier * dir;
  if (is_ball_inside_box(temp, radius, box_vertices)) {
    // printf("Ball is inside the box\n");
    center = temp;
    double angle = 360 * usual_dist_change / (2 * M_PI * radius);
    rotate_ball_vertices(right, -angle);
  } else {
    // collision will happen
    std::vector<std::pair<int, double>> collision_distances =
        get_collision_distances();
    double dist_to_collision = collision_distances[0].second;
    // printf("Collision distances:\n");
    // for (int i = 0; i < collision_distances.size(); i++) {
    //   printf("Wall %d: %lf\n", collision_distances[i].first,
    //          collision_distances[i].second);
    // }

    // first reach to wall
    center += std::min(1.00, dist_to_collision / usual_dist_change) *
              dir_scalar_multiplier * dir;

    // testing
    double now_dist = get_distance_from_wall(collision_distances[0].first);
    printf("Now dist: %lf\n", now_dist);

    double angle = 360 * dist_to_collision / (2 * M_PI * radius);
    rotate_ball_vertices(right, -angle);

    // now collision has taken place
    // update the direction vectors after collision
    if (fabs(collision_distances[0].second - collision_distances[1].second) <
        EPS) {
      // corner collision
      printf("Corner collision\n");
      dir.x *= -1;
      dir.y *= -1;
      right.x *= -1;
      right.y *= -1;
      up = right.cross(dir);
    } else {
      int wall_idx = collision_distances[0].first;
      if (fabs(box_vertices[wall_idx].x - box_vertices[wall_idx + 1].x) < EPS) {
        // wall parallel to y-axis
        printf("Wall parallel to y-axis\n");
        dir.x *= -1;
        right.x *= -1;
        up = right.cross(dir);
      } else {
        // wall parallel to x-axis
        printf("Wall parallel to x-axis\n");
        dir.y *= -1;
        right.y *= -1;
        up = right.cross(dir);
      }
    }

    // now the remaining movement and collision should be done
    // fraction for the remaining movement
    printf("Usual Dist Change: %lf, Dist to collision: %lf\n",
           usual_dist_change, dist_to_collision);
    double fraction =
        fabs(usual_dist_change - dist_to_collision) / usual_dist_change;
    center += fraction * dir_scalar_multiplier * dir;
    angle = 360 * fraction * usual_dist_change / (2 * M_PI * radius);
    rotate_ball_vertices(right, -angle);
    printf("Done with collision\n");
  }
}

void Ball::go_backward() {
  dir = dir * -1;
  right = right * -1;
  up = right.cross(dir);
  go_forward();
  dir = dir * -1;
  right = right * -1;
  up = right.cross(dir);
}

bool Ball::is_ball_inside_box(Point3D c, double rad,
                              const std::vector<Point3D>& box_vertices) {
  std::pair<double, double> x_coordinate = {INF, -INF};  // <min, max>
  std::pair<double, double> y_coordinate = {INF, -INF};  // <min, max>
  for (int i = 0; i < box_vertices.size(); i++) {
    x_coordinate = {std::min(x_coordinate.first, box_vertices[i].x),
                    std::max(x_coordinate.second, box_vertices[i].x)};
    y_coordinate = {std::min(y_coordinate.first, box_vertices[i].y),
                    std::max(y_coordinate.second, box_vertices[i].y)};
  }
  // printf("Ball: (%lf, %lf, %lf)\n", c.x, c.y, c.z);
  // printf("Box: (%lf, %lf) (%lf, %lf)\n", x_coordinate.first,
  //        x_coordinate.second, y_coordinate.first, y_coordinate.second);
  if (c.x - rad < x_coordinate.first || c.x + rad > x_coordinate.second ||
      c.y - rad < y_coordinate.first || c.y + rad > y_coordinate.second)
    return false;
  return true;
}

void Ball::rotate_ball_vertices(const Vector& axis, double angle) {
  for (int i = 0; i < ball_vertices.size(); i++) {
    Vector v =
        Vector(ball_vertices[i].x, ball_vertices[i].y, ball_vertices[i].z);
    v = v.rotate(axis, angle);
    ball_vertices[i] = Point3D(v.x, v.y, v.z);
  }
}

void Ball::rotate_dir_ccw() {
  dir = dir.rotate(up, dir_rotation_angle);
  right = right.rotate(up, dir_rotation_angle);
}

void Ball::rotate_dir_cw() {
  dir = dir.rotate(up, -dir_rotation_angle);
  right = right.rotate(up, -dir_rotation_angle);
}

/*
Assumption:
1. The walls of the box are aligned to the x and y axes
2. As a result of 1, there will only be 4 walls
*/

std::vector<std::pair<int, double>> Ball::get_collision_distances() {
  std::vector<std::pair<int, double>>
      collision_distances;  // <wall_index, distance>
  for (int i = 0; i < box_vertices.size() - 1; i++) {
    double dist = get_distance_from_wall(i);
    collision_distances.push_back(std::make_pair(i, dist));
  }
  std::sort(
      collision_distances.begin(), collision_distances.end(),
      [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
        return a.second < b.second;
      });
  return collision_distances;
}

// this function returns the distance from the ball to the wall along the
// direction vector of the ball
double Ball::get_distance_from_wall(int wall_idx) {
  Point3D p1 = box_vertices[wall_idx];
  Point3D p2 = box_vertices[wall_idx + 1];

  double center_to_wall_distance;

  // the straight line connecting center and the direction vector of the ball
  // will be: dir.y * x - dir.x * y = dir.y * center.x - dir.x * center.y
  printf("Line Equation: %lfx - %lfy = %lf\n", dir.y, dir.x,
         dir.y * center.x - dir.x * center.y);
  if (fabs(p1.x - p2.x) < EPS) {  // wall parallel to y-axis
    double perp_distance_ball_center = fabs(p1.x - center.x);
    double y_collision_point =
        (dir.y * center.x - dir.x * center.y - dir.y * p1.x) / (-dir.x);
    double dist_perp_point_collision_point = fabs(y_collision_point - center.y);
    center_to_wall_distance =
        sqrt(perp_distance_ball_center * perp_distance_ball_center +
             dist_perp_point_collision_point * dist_perp_point_collision_point);
    printf(
        "Perp. distance: %lf, Perp. point to collision point distance: %lf\n, "
        "Center to wall distance: "
        "%lf\n",
        perp_distance_ball_center, dist_perp_point_collision_point,
        center_to_wall_distance);
  } else {  // wall parallel to x-axis
    double perp_distance_ball_center = fabs(p1.y - center.y);
    double x_collision_point =
        (dir.y * center.x - dir.x * center.y + dir.x * p1.y) / (dir.y);
    double dist_perp_point_collision_point = fabs(x_collision_point - center.x);
    center_to_wall_distance =
        sqrt(perp_distance_ball_center * perp_distance_ball_center +
             dist_perp_point_collision_point * dist_perp_point_collision_point);
    printf(
        "Perp. distance: %lf, Perp. point to collision point distance: %lf\n, "
        "Center to wall distance: "
        "%lf\n",
        perp_distance_ball_center, dist_perp_point_collision_point,
        center_to_wall_distance);
  }
  printf(
      "Wall p1: (%lf, %lf, %lf), p2: (%lf, %lf, %lf), center: (%lf, %lf, "
      "%lf), distance: %lf\n",
      p1.x, p1.y, p1.z, p2.x, p2.y, p2.z, center.x, center.y, center.z,
      center_to_wall_distance);
  return center_to_wall_distance - radius;
}