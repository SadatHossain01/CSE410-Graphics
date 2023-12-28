#include <cctype>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stack>
#include <string>

#include "bitmap_image.hpp"
#include "matrix.h"
#include "transform.h"
#include "triangle.h"
#include "vector.h"

// Camera Inputs
Vector eye, look, up;  // Position vectors of the three points
double fov_y, aspect_ratio, near, far;
// Config Inputs
double screen_width, screen_height;

std::vector<Triangle> triangles;

int main(int argc, char** argv) {
  // input files
  std::string scene_file_name, config_file_name;
  scene_file_name = (argc > 1) ? argv[1] : "scene.txt";
  config_file_name = (argc > 2) ? argv[2] : "config.txt";
  std::ifstream scene_file(scene_file_name);
  std::ifstream config_file(config_file_name);

  // output files
  std::string stage1_filename = "stage1.txt";
  std::string stage2_filename = "stage2.txt";
  std::string stage3_filename = "stage3.txt";
  std::string stage4_filename = "z_buffer.txt";
  std::ofstream stage1_file(stage1_filename);
  std::ofstream stage2_file(stage2_filename);
  std::ofstream stage3_file(stage3_filename);
  std::ofstream stage4_file(stage4_filename);

  stage1_file << std::fixed << std::setprecision(7);
  stage2_file << std::fixed << std::setprecision(7);
  stage3_file << std::fixed << std::setprecision(7);
  stage4_file << std::fixed << std::setprecision(6);

  // Camera Inputs
  scene_file >> eye >> look >> up >> fov_y >> aspect_ratio >> near >> far;

  // Stage 1 : Modeling Transformation
  std::stack<Matrix> s;
  s.push(Matrix::identity(4));

  while (true) {
    scene_file.ignore(256, '\n');
    std::string command;
    // std::getline(scene_file, command);
    scene_file >> command;
    for (char& c : command) c = tolower(c);

    if (command == "triangle") {
      Triangle triangle;
      scene_file >> triangle;
      triangle.transform(s.top());
      triangles.push_back(triangle);
      stage1_file << triangle << std::endl;
      stage1_file << std::endl;
    } else if (command == "translate") {
      double tx, ty, tz;
      scene_file >> tx >> ty >> tz;
      Matrix translation_matrix =
          Transformation::generate_translation_matrix(tx, ty, tz);
      s.top() = s.top() * translation_matrix;
    } else if (command == "scale") {
      double sx, sy, sz;
      scene_file >> sx >> sy >> sz;
      Matrix scaling_matrix =
          Transformation::generate_scaling_matrix(sx, sy, sz);
      s.top() = s.top() * scaling_matrix;
    } else if (command == "rotate") {
      double angle, rx, ry, rz;
      scene_file >> angle >> rx >> ry >> rz;
      Matrix rotation_matrix =
          Transformation::generate_rotation_matrix(rx, ry, rz, angle);
      s.top() = s.top() * rotation_matrix;
    } else if (command == "push") {
      s.push(s.top());
    } else if (command == "pop") {
      s.pop();
    } else if (command == "end") {
      break;
    } else {
      std::cerr << "Invalid command: " << command << std::endl;
      return 1;
    }
  }

  // Stage 2 : Viewing Transformation
  Matrix view = Transformation::generate_view_matrix(eye, look, up);
  for (Triangle& triangle : triangles) {
    triangle.transform(view);
    stage2_file << triangle << std::endl;
    stage2_file << std::endl;
  }

  // Stage 3 : Projection Transformation
  Matrix projection = Transformation::generate_projection_matrix(
      fov_y, aspect_ratio, near, far);
  for (Triangle& triangle : triangles) {
    triangle.transform(projection);
    stage3_file << triangle << std::endl;
    stage3_file << std::endl;
  }

  scene_file.close();
  stage1_file.close();
  stage2_file.close();
  stage3_file.close();

  // Stage 4 : Clipping and Scan Conversion
  config_file >> screen_width >> screen_height;

  // Assign colors to triangles
  for (Triangle& triangle : triangles) {
    triangle.set_random_colors();
  }

  double z_min = -1.0, z_max = 1.0;
  double left_limit = -1.0, right_limit = 1.0;
  double bottom_limit = -1.0, top_limit = 1.0;
  double dx = (right_limit - left_limit) / screen_width;
  double dy = (top_limit - bottom_limit) / screen_height;
  double top_y = top_limit - dy / 2.0;
  double bottom_y = bottom_limit + dy / 2.0;
  double left_x = left_limit + dx / 2.0;
  double right_x = right_limit - dx / 2.0;

  std::vector<std::vector<double>> z_buffer(
      screen_height, std::vector<double>(screen_width, z_max));
  bitmap_image image(screen_width, screen_height);
  image.set_all_channels(0, 0, 0);

  // Procedure
  for (Triangle& triangle : triangles) {
    triangle.reorder_vertices();

    // the 3 vertices of the triangle
    Vector a =
        Vector(triangle.vertices[0].data[0][0], triangle.vertices[0].data[1][0],
               triangle.vertices[0].data[2][0]);
    Vector b =
        Vector(triangle.vertices[1].data[0][0], triangle.vertices[1].data[1][0],
               triangle.vertices[1].data[2][0]);
    Vector c =
        Vector(triangle.vertices[2].data[0][0], triangle.vertices[2].data[1][0],
               triangle.vertices[2].data[2][0]);

    double bottom_scanline = std::max(c.y, bottom_y);
    double top_scanline = std::min(a.y, top_y);

    int top_row = round((top_scanline - bottom_y) / dy);
    int bottom_row = round((bottom_scanline - bottom_y) / dy);

    for (int i = top_row; i >= bottom_row; i--) {
      double y_s = bottom_y + i * dy;
      double x_a = a.x - (a.x - b.x) * (a.y - y_s) / (a.y - b.y);
      double x_b = a.x - (a.x - c.x) * (a.y - y_s) / (a.y - c.y);
      double z_a = a.z - (a.z - b.z) * (a.y - y_s) / (a.y - b.y);
      double z_b = a.z - (a.z - c.z) * (a.y - y_s) / (a.y - c.y);

      x_a = std::max(x_a, left_x);
      x_b = std::min(x_b, right_x);

      int left_col = round((x_a - left_x) / dx);
      int right_col = round((x_b - left_x) / dx);

      for (int j = left_col; j <= right_col; j++) {
        double x_p = left_x + j * dx;
        double z_p = z_b - (z_b - z_a) * (x_b - x_p) / (x_b - x_a);

        if (z_p >= z_min && z_p < z_buffer[screen_height - 1 - i][j]) {
          z_buffer[screen_height - 1 - i][j] = z_p;
          image.set_pixel(j, screen_height - 1 - i, triangle.red,
                          triangle.green, triangle.blue);
        }
      }
    }
  }

  image.save_image("out.bmp");

  for (int i = 0; i < screen_height; i++) {
    for (int j = 0; j < screen_width; j++) {
      if (z_buffer[i][j] >= z_max) continue;
      stage4_file << z_buffer[i][j] << "\t";
    }
    stage4_file << std::endl;
  }

  image.clear();
  z_buffer.clear();

  // Close files
  config_file.close();
  stage4_file.close();
  return 0;
}