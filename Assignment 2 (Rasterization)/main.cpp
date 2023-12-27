#include <cctype>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stack>
#include <string>

#include "matrix.h"
#include "transform.h"
#include "triangle.h"
#include "vector.h"

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
  std::ofstream stage1_file(stage1_filename);
  std::ofstream stage2_file(stage2_filename);
  std::ofstream stage3_file(stage3_filename);

  stage1_file << std::fixed << std::setprecision(7);
  stage2_file << std::fixed << std::setprecision(7);
  stage3_file << std::fixed << std::setprecision(7);

  // Camera Inputs
  Vector eye, look, up;  // Position vectors of the three points
  double fov_y, aspect_ratio, near, far;
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
      s.top() = translation_matrix * s.top();
    } else if (command == "scale") {
      double sx, sy, sz;
      scene_file >> sx >> sy >> sz;
      Matrix scaling_matrix =
          Transformation::generate_scaling_matrix(sx, sy, sz);
      s.top() = scaling_matrix * s.top();
    } else if (command == "rotate") {
      double angle, rx, ry, rz;
      scene_file >> angle >> rx >> ry >> rz;
      Matrix rotation_matrix =
          Transformation::generate_rotation_matrix(rx, ry, rz, angle);
      s.top() = rotation_matrix * s.top();
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

    std::cout << command << ":" << std::endl;
    std::cout << s.top() << std::endl << std::endl;
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

  // Close files
  scene_file.close();
  config_file.close();
  stage1_file.close();
  stage2_file.close();
  stage3_file.close();
  return 0;
}