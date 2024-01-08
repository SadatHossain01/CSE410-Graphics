#include <cctype>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stack>
#include <string>
#include <tuple>

#include "bitmap_image.hpp"
#include "line.h"
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

// Function Declarations
std::pair<bool, Point> check_line_segment_intersection(const Line& line,
                                                       Line segment);
bool is_equal(double a, double b);

// Function Definitions
std::pair<bool, Point> check_line_segment_intersection(const Line& line,
                                                       Line segment) {
  std::pair<bool, Point> intersection_point =
      line.get_intersection_point(segment);
  if (!intersection_point.first) return intersection_point;
  if (segment.p0.x > segment.p1.x) std::swap(segment.p0, segment.p1);
  if (is_equal(segment.p0.x, segment.p1.x)) {
    intersection_point.first =
        intersection_point.second.y >= std::min(segment.p0.y, segment.p1.y) &&
        intersection_point.second.y <= std::max(segment.p0.y, segment.p1.y);
  } else {
    intersection_point.first = intersection_point.second.x >= segment.p0.x &&
                               intersection_point.second.x <= segment.p1.x;
  }
  return intersection_point;
}
bool is_equal(double a, double b) {
  return fabs(a - b) <= std::numeric_limits<double>::epsilon();
}

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
  for (int tr = 0; tr < triangles.size(); tr++) {
    Triangle triangle = triangles[tr];
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

    // the three edges of the triangle
    // taking z = 0 plane as the projection plane (xy plane)
    Line l1 = Line(Point(a.x, a.y, 0), Point(b.x, b.y, 0));
    Line l2 = Line(Point(a.x, a.y, 0), Point(c.x, c.y, 0));
    Line l3 = Line(Point(b.x, b.y, 0), Point(c.x, c.y, 0));

    double bottom_scanline = std::max(c.y, bottom_y);
    double top_scanline = std::min(a.y, top_y);

    int top_row = round((top_scanline - bottom_y) / dy);
    int bottom_row = round((bottom_scanline - bottom_y) / dy);

    for (int i = top_row; i >= bottom_row; i--) {
      double y_s = bottom_y + i * dy;

      Line current_line = Line(Point(0, y_s, 0), Point(1, y_s, 0));

      std::pair<bool, Point> l1_intersection =
          check_line_segment_intersection(current_line, l1);
      std::pair<bool, Point> l2_intersection =
          check_line_segment_intersection(current_line, l2);
      std::pair<bool, Point> l3_intersection =
          check_line_segment_intersection(current_line, l3);

      int intersection_count =
          l1_intersection.first + l2_intersection.first + l3_intersection.first;

      // if (intersection_count == 3) std::cerr << "should not happen\n";

      auto reorder_points_and_lines = [&](std::string order) {
        if (order[1] > order[2]) std::swap(order[1], order[2]);
        if (order == "abc") {
          // keep things as they are
        } else if (order == "bac") {
          std::tie(a, b, c) = std::make_tuple(b, a, c);
          std::tie(l1, l2, l3) = std::make_tuple(l1, l3, l2);
          std::tie(l1_intersection, l2_intersection, l3_intersection) =
              std::make_tuple(l1_intersection, l3_intersection,
                              l2_intersection);
        } else if (order == "cab") {
          std::tie(a, b, c) = std::make_tuple(c, a, b);
          std::tie(l1, l2, l3) = std::make_tuple(l2, l3, l1);
          std::tie(l1_intersection, l2_intersection, l3_intersection) =
              std::make_tuple(l2_intersection, l3_intersection,
                              l1_intersection);
        }
      };
      if (intersection_count == 0) continue;
      if (intersection_count == 2) {
        reorder_points_and_lines(l1_intersection.first
                                     ? (l2_intersection.first ? "abc" : "bac")
                                     : "cab");
      }

      // double x_a = a.x - (a.x - b.x) * (a.y - y_s) / (a.y - b.y);
      // double x_b = a.x - (a.x - c.x) * (a.y - y_s) / (a.y - c.y);
      double x_a = l1_intersection.second.x;
      double x_b = l2_intersection.second.x;
      double z_a = a.z - (a.z - b.z) * (a.y - y_s) / (a.y - b.y);
      double z_b = a.z - (a.z - c.z) * (a.y - y_s) / (a.y - c.y);

      if (x_a > x_b) {
        std::swap(x_a, x_b);
        std::swap(z_a, z_b);
      }

      int left_col = round((std::max(x_a, left_x) - left_x) / dx);
      int right_col = round((std::min(x_b, right_x) - left_x) / dx);

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