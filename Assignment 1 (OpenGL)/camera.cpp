#include "camera.h"

#include <cassert>
#include <cmath>

Camera::Camera() {
  // initialize the camera with appropriate values
  pos = Point3D(20, 20, 40);
  look =
      Vector(-pos.x, -pos.y, -pos.z)
          .normalize();  // so, looking at origin (which is the reference point)
  right = Vector(-5, 5, 0).normalize();
  up = right.cross(look).normalize();
}

Camera::Camera(Point3D eye, Vector u, Vector v, Vector w)
    : pos(eye), up(u), right(v), look(w) {
  assert(u.check_normalized() && v.check_normalized() && w.check_normalized());
  assert(u.check_orthogonal(v) && v.check_orthogonal(w) &&
         w.check_orthogonal(u));
}

void Camera::move_forward() { pos += speed * look; }
void Camera::move_backward() { pos -= speed * look; }
void Camera::move_left() { pos -= speed * right; }
void Camera::move_right() { pos += speed * right; }
void Camera::move_up() { pos += speed * up; }
void Camera::move_down() { pos -= speed * up; }
void Camera::look_left() {
  look = look.rotate(up, rotation_speed);
  right = right.rotate(up, rotation_speed);
}
void Camera::look_right() {
  look = look.rotate(up, -rotation_speed);
  right = right.rotate(up, -rotation_speed);
}
void Camera::look_up() {
  look = look.rotate(right, rotation_speed);
  up = up.rotate(right, rotation_speed);
}
void Camera::look_down() {
  look = look.rotate(right, -rotation_speed);
  up = up.rotate(right, -rotation_speed);
}
void Camera::tilt_clockwise() {
  right = right.rotate(look, rotation_speed);
  up = up.rotate(look, rotation_speed);
}
void Camera::tilt_counterclockwise() {
  right = right.rotate(look, -rotation_speed);
  up = up.rotate(look, -rotation_speed);
}
void Camera::move_up_same_ref() {}
void Camera::move_down_same_ref() {}
