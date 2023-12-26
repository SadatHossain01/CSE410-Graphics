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
  center = Point3D(0, 0, radius);
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
    }
  }
}

void Ball::go_forward() {
  double usual_dist_change = dir_scalar_multiplier * dir.norm();
  Point3D temp = center + dir_scalar_multiplier * dir;
  if (is_ball_inside_box(temp, radius, box_vertices)) {
    center = temp;
    double angle = 360 * usual_dist_change / (2 * M_PI * radius);
    rotate_ball_vertices(right, -angle);
  } else {
    // collision will happen
    std::vector<std::pair<int, double>> collision_distances =
        get_collision_distances();
    std::sort(
        collision_distances.begin(), collision_distances.end(),
        [](const std::pair<int, double> &a, const std::pair<int, double> &b) {
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

    update_direction_after_collision(collision_distances[0].first);

    // now the remaining movement and collision should be done
    // fraction for the remaining movement
    double fraction =
        fabs(usual_dist_change - dist_to_collision) / usual_dist_change;
    center += fraction * dir_scalar_multiplier * dir;
    angle = 360 * fraction * usual_dist_change / (2 * M_PI * radius);
    rotate_ball_vertices(right, -angle);
  }
}

void Ball::update_direction_after_collision(int wall_idx) {
  if (wall_idx == -1) {
    std::vector<std::pair<int, double>> collision_distances =
        get_collision_distances();
    std::sort(
        collision_distances.begin(), collision_distances.end(),
        [](const std::pair<int, double> &a, const std::pair<int, double> &b) {
          return a.second < b.second;
        });
    wall_idx = collision_distances[0].first;
  }
  Vector wall_vector =
      Vector(box_vertices[wall_idx + 1].x - box_vertices[wall_idx].x,
             box_vertices[wall_idx + 1].y - box_vertices[wall_idx].y,
             box_vertices[wall_idx + 1].z - box_vertices[wall_idx].z);
  Vector normal = wall_vector.cross(Vector(0, 0, 1)).normalize();

  dir = dir.get_reflection(normal).normalize();
  right = dir.cross(up);
  up = right.cross(dir);
}

void Ball::go_backward() {
  dir = dir * -1;
  right = dir.rotate(up, -90);
  go_forward();
  dir = dir * -1;
  right = dir.rotate(up, -90);
}

bool Ball::is_ball_inside_box(Point3D c, double rad,
                              const std::vector<Point3D> &box_vertices) {
  std::pair<double, double> x_coordinate = {INF, -INF};  // <min, max>
  std::pair<double, double> y_coordinate = {INF, -INF};  // <min, max>
  for (int i = 0; i < box_vertices.size() - 1; i++) {
    Line l(box_vertices[i], box_vertices[i + 1]);
    double dist = l.get_distance_from_point(c);
    if (dist < rad - EPS) return false;
  }
  return true;
}

void Ball::rotate_ball_vertices(const Vector &axis, double angle) {
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

  Line l(p1, p2);

  // First need to check if the ball is heading towards the wall or opposite
  // direction
  Point3D temp = center - dir_scalar_multiplier * dir;
  double dist_temp = l.get_distance_from_point_along_vector(temp, dir);
  double dist_along_dir = l.get_distance_from_point_along_vector(center, dir);

  if (dist_temp < dist_along_dir) return INF;  // to invalidate this collision

  double perp_distance = l.get_distance_from_point(center);
  double dist_at_collision = (dist_along_dir * radius) / perp_distance;
  double distance_till_collision = dist_along_dir - dist_at_collision;

  return std::max(0.0, distance_till_collision);  // using the real distance
}

double Ball::next_collision_time() {
  std::vector<std::pair<int, double>> collision_distances =
      get_collision_distances();
  std::sort(
      collision_distances.begin(), collision_distances.end(),
      [](const std::pair<int, double> &a, const std::pair<int, double> &b) {
        return a.second < b.second;
      });
  double dist = collision_distances[0].second;
  double time = dist / ((dir_scalar_multiplier * dir.norm()) / dt);  // in ms
  return time;
}