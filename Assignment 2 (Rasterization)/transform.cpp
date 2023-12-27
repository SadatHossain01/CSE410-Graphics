#include "transform.h"

#include <cmath>

#include "vector.h"

const double PI = 2 * acos(0.0);

Matrix Transformation::generate_translation_matrix(double x, double y,
                                                   double z) {
  Matrix result = Matrix::identity(4);
  result.data[0][3] = x;
  result.data[1][3] = y;
  result.data[2][3] = z;
  return result;
}

Matrix Transformation::generate_scaling_matrix(double x, double y, double z) {
  Matrix result = Matrix::identity(4);
  result.data[0][0] = x;
  result.data[1][1] = y;
  result.data[2][2] = z;
  return result;
}

Matrix Transformation::generate_rotation_matrix(double x, double y, double z,
                                                double angle) {
  Vector axis = Vector(x, y, z).normalize();

  std::vector<Vector> bases(3);
  bases[0] = Vector(1, 0, 0);
  bases[1] = Vector(0, 1, 0);
  bases[2] = Vector(0, 0, 1);

  Matrix result = Matrix::identity(4);
  for (int i = 0; i < 3; i++) {
    Vector v = bases[i];
    Vector v_rotated = v.rotate(axis, angle);
    result.data[0][i] = v_rotated.x;
    result.data[1][i] = v_rotated.y;
    result.data[2][i] = v_rotated.z;
  }

  return result;
}

Matrix Transformation::generate_view_matrix(Vector eye, Vector look,
                                            Vector up) {
  Vector look_vector = look - eye;
  look_vector = look_vector.normalize();
  Vector right_vector = look_vector.cross(up);
  right_vector = right_vector.normalize();
  Vector up_vector = right_vector.cross(look_vector).normalize();

  Matrix view =
      Transformation::generate_translation_matrix(-eye.x, -eye.y, -eye.z);
  Matrix rotation(4, 4);
  rotation.data[0][0] = right_vector.x, rotation.data[0][1] = right_vector.y,
  rotation.data[0][2] = right_vector.z;
  rotation.data[1][0] = up_vector.x, rotation.data[1][1] = up_vector.y,
  rotation.data[1][2] = up_vector.z;
  rotation.data[2][0] = -look_vector.x, rotation.data[2][1] = -look_vector.y,
  rotation.data[2][2] = -look_vector.z;
  rotation.data[3][3] = 1;
  view = rotation * view;
  return view;
}

Matrix Transformation::generate_projection_matrix(double fov_y,
                                                  double aspect_ratio,
                                                  double near, double far) {
  double fov_x = fov_y * aspect_ratio;
  double t = near * tan(fov_y * PI / 360.0);
  double r = near * tan(fov_x * PI / 360.0);
  Matrix projection(4, 4);
  projection.data[0][0] = near / r;
  projection.data[1][1] = near / t;
  projection.data[2][2] = -(far + near) / (far - near);
  projection.data[2][3] = -(2 * far * near) / (far - near);
  projection.data[3][2] = -1;
  return projection;
}