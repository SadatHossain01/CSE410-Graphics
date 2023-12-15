#include "ball.h"

#include <algorithm>
#include <cassert>
#include <cmath>

const double INF = 1e9;
const double EPS = 1e-6;

Ball::Ball(double radius, int sector_count, int stack_count)
    : radius(radius), sector_count(sector_count), stack_count(stack_count) {
  dir_rotation_angle = dir_scalar_multiplier * 180 / (radius * M_PI);
  dir = Vector(1, 1, 0).normalize();
  up = Vector(0, 0, 1).normalize();
  right = dir.cross(up);
  center = Point3D(0, 0, 5);
  compute_vertices();
}

// Vertices are computed here considering the origin (0, 0, 0) as the center
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
    std::sort(
        collision_distances.begin(), collision_distances.end(),
        [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
          return a.second < b.second;
        });
    double dist_to_collision = collision_distances[0].second;

    // first reach to wall
    center +=
        (dist_to_collision / usual_dist_change) * dir_scalar_multiplier * dir;

    double angle = 360 * dist_to_collision / (2 * M_PI * radius);
    rotate_ball_vertices(right, -angle);

    // now collision has taken place
    // update the direction vectors after collision
    if (fabs(collision_distances[0].second - collision_distances[1].second) <
        EPS) {
      // corner collision
      dir.x *= -1;
      dir.y *= -1;
      right = dir.rotate(up, -90);
    } else {
      int wall_idx = collision_distances[0].first;
      if (fabs(box_vertices[wall_idx].x - box_vertices[wall_idx + 1].x) < EPS) {
        // wall parallel to y-axis
        dir.x *= -1;
        right = dir.rotate(up, -90);
      } else {
        // wall parallel to x-axis
        dir.y *= -1;
        right = dir.rotate(up, -90);
      }
    }

    // now the remaining movement and collision should be done
    // fraction for the remaining movement
    double fraction =
        fabs(usual_dist_change - dist_to_collision) / usual_dist_change;
    center += fraction * dir_scalar_multiplier * dir;
    angle = 360 * fraction * usual_dist_change / (2 * M_PI * radius);
    rotate_ball_vertices(right, -angle);
  }
}

void Ball::go_backward() {
  dir = dir * -1;
  right = dir.rotate(up, -90);
  go_forward();
  dir = dir * -1;
  right = dir.rotate(up, -90);
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
Assumptions:
1. The walls of the box are aligned to the x and y axes
2. As a result of 1, there will only be 4 walls
*/
// Does not return sorted vector
std::vector<std::pair<int, double>> Ball::get_collision_distances() {
  std::vector<std::pair<int, double>>
      collision_distances;  // <wall_index, distance>
  for (int i = 0; i < box_vertices.size() - 1; i++) {
    double dist = get_distance_from_wall(i);
    collision_distances.push_back(std::make_pair(i, dist));
  }
  return collision_distances;
}

// Returns the distance that the ball would be able to travel along the dir
// vector before collision
double Ball::get_distance_from_wall(int wall_idx) {
  Point3D p1 = box_vertices[wall_idx];
  Point3D p2 = box_vertices[wall_idx + 1];

  double distance_till_collision;

  // the straight line connecting center and the direction vector of the ball
  // will be: dir.y * x - dir.x * y = dir.y * center.x - dir.x * center.y

  /*
  Point a: the point at which the line perpendicular to the plane passing
  through the center would intersect the plane
  Point b: the point at which the line along the direction vector passing
  through the center would intersect the plane
  */
  // We also need to find out if the ball is heading towards that wall or
  // opposite direction
  // if opposite direction, then the collision will not happen soon
  if (fabs(p1.x - p2.x) < EPS) {  // wall parallel to y-axis
    double t = (p1.x - center.x) / dir.x;
    if (t < -EPS) return INF;  // to invalidate this collision
    double a_to_center_distance = fabs(p1.x - center.x);
    double y_coordinate_of_b =
        (dir.y * center.x - dir.x * center.y - dir.y * p1.x) / (-dir.x);
    double a_to_b_distance = fabs(y_coordinate_of_b - center.y);
    double center_to_wall_distance =
        sqrt(a_to_center_distance * a_to_center_distance +
             a_to_b_distance * a_to_b_distance);
    double distance_at_collision_moment =
        (center_to_wall_distance * radius) / a_to_center_distance;
    distance_till_collision =
        center_to_wall_distance - distance_at_collision_moment;
  } else {  // wall parallel to x-axis
    double t = (p1.y - center.y) / dir.y;
    if (t < -EPS) return INF;  // to invalidate this collision
    double a_to_center_distance = fabs(p1.y - center.y);
    double x_coordinate_of_b =
        (dir.y * center.x - dir.x * center.y + dir.x * p1.y) / (dir.y);
    double a_to_b_distance = fabs(x_coordinate_of_b - center.x);
    double center_to_wall_distance =
        sqrt(a_to_center_distance * a_to_center_distance +
             a_to_b_distance * a_to_b_distance);
    double distance_at_collision_moment =
        (center_to_wall_distance * radius) / a_to_center_distance;
    distance_till_collision =
        center_to_wall_distance - distance_at_collision_moment;
  }
  /*
  center_to_wall_distance is the distance from the ball's center to the wall
  along the direction vector. However, the ball would not be able to go
  uninterrupted to that distance because the collision would take place even
  before. So how much distance along that direction will the ball be able to go?
  It will be able to go as much as the perpendicular distance from the ball's
  center to the wall is equal to the radius. But both the triangles are similar.
  So, we can use the ratio to find out that till-collision-distance.
  */

  return std::max(0.0, distance_till_collision);  // using the real distance
}

/*
Return values:
-1: no collision
1: collision with wall parallel to y-axis
2: collision with wall parallel to x-axis
3: corner collision
*/
int Ball::get_collision_type() {
  Point3D temp = center + dir_scalar_multiplier * dir;
  if (is_ball_inside_box(temp, radius, box_vertices)) return -1;
  std::vector<std::pair<int, double>> collision_distances =
      get_collision_distances();
  std::sort(
      collision_distances.begin(), collision_distances.end(),
      [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
        return a.second < b.second;
      });
  if (fabs(collision_distances[0].second - collision_distances[1].second) < 0.5)
    return 3;  // corner collision
  else {
    int wall_idx = collision_distances[0].first;
    if (fabs(box_vertices[wall_idx].x - box_vertices[wall_idx + 1].x) < EPS)
      // wall parallel to y-axis
      return 1;
    else
      // wall parallel to x-axis
      return 2;
  }
}

int Ball::get_facing_wall_idx() {
  // The direction vector passes through the center of the ball
  // so the points passed by the current trajectory will be:
  // x = center.x + dir.x * t
  // y = center.y + dir.y * t

  for (int i = 0; i < box_vertices.size() - 1; i++) {
    Point3D p1 = box_vertices[i];
    Point3D p2 = box_vertices[i + 1];
    // printf("Wall %d: (%lf, %lf) (%lf, %lf)\n", i, p1.x, p1.y, p2.x, p2.y);
    // printf("Center: (%lf, %lf)\n", center.x, center.y);
    // printf("Dir: (%lf, %lf)\n", dir.x, dir.y);
    if (fabs(p1.x - p2.x) < EPS) {  // wall parallel to y-axis
      // the wall's equation will be, x = p1.x
      double t = (p1.x - center.x) / dir.x;
      // printf("parallel to y: t: %lf\n", t);
      if (t < -EPS) continue;
      double y = center.y + dir.y * t;
      // printf("x: %lf, y: %lf\n", center.x + dir.x * t, y);
      if (y >= std::min(p1.y, p2.y) - EPS && y <= std::max(p1.y, p2.y) + EPS)
        return i;
    } else {  // wall parallel to x-axis
      // the wall's equation will be, y = p1.y
      double t = (p1.y - center.y) / dir.y;
      // printf("parallel to x: t: %lf\n", t);
      if (t < -EPS) continue;
      double x = center.x + dir.x * t;
      // printf("x: %lf, y: %lf\n", x, center.y + dir.y * t);
      if (x >= std::min(p1.x, p2.x) - EPS && x <= std::max(p1.x, p2.x) + EPS)
        return i;
    }
  }

  std::vector<std::pair<int, double>> collision_distances =
      get_collision_distances();
  std::sort(
      collision_distances.begin(), collision_distances.end(),
      [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
        return a.second < b.second;
      });
  return collision_distances[0].first;
}

double Ball::next_collision_time() {
  std::vector<std::pair<int, double>> collision_distances =
      get_collision_distances();
  std::sort(
      collision_distances.begin(), collision_distances.end(),
      [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
        return a.second < b.second;
      });
  double dist = collision_distances[0].second;
  double time = dist / ((dir_scalar_multiplier * dir.norm()) / dt);  // in ms
  return time;
}