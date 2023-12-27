#ifndef TRANSFORM_H
#define TRANSFORM_H
#endif

#include "matrix.h"
#include "vector.h"

class Transformation {
 public:
  static Matrix generate_translation_matrix(double x, double y, double z);
  static Matrix generate_scaling_matrix(double x, double y, double z);
  static Matrix generate_rotation_matrix(double x, double y, double z,
                                         double angle);  // angle in degrees
  // eye, look and up are position vectors for the three points
  static Matrix generate_view_matrix(Vector eye, Vector look, Vector up);
  static Matrix generate_projection_matrix(double fov_y, double aspect_ratio,
                                           double near, double far);
};