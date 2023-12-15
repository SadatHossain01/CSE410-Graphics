#ifndef CAMERA_H
#define CAMERA_H

#include "point3D.h"
#include "vector.h"

struct Camera {
 public:
  double speed = 2;           // for movement operations
  double rotation_speed = 1;  // in degrees
  Point3D pos;
  Vector up, right, look;  // normalized vectors

  Camera(const Point3D& pos = Point3D(5, 5, 5),
         const Point3D& obj = Point3D(0, 0, 0),
         const Vector& up =
             Vector(0, 0, 1));  // initialize the camera with appropriate values
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

  void move_up_same_ref();
  void move_down_same_ref();
};

#endif