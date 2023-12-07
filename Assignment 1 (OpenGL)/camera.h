#ifndef CAMERA_H
#define CAMERA_H

#include "point3D.h"
#include "vector.h"

struct Camera {
 private:
  double speed = 0.5;         // for movement operations
  double rotation_speed = 1;  // in degrees
 public:
  Point3D pos;
  Vector up, right, look;  // normalized vectors

  Camera();  // initialize the camera with appropriate values
  Camera(Point3D eye, Vector u, Vector v, Vector w);
  void move_forward();
  void move_backward();
  void move_left();
  void move_right();
  void move_up();
  void move_down();
  //   camera rotation functions
  void look_left();
  void look_right();
  void look_up();
  void look_down();
  void tilt_clockwise();
  void tilt_counterclockwise();
};

#endif