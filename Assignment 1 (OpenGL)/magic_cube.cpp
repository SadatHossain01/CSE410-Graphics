#include <bits/stdc++.h>

#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <glut.h>
#include <windows.h>
#endif

#include "ball.h"
#include "camera.h"
#include "point3D.h"

Camera camera(Point3D(5, 0, 2), Point3D(0, 0, 0), Vector(0, 0, 1));

// Triangle
double max_triangle_length = 2.0;
double triangle_length = max_triangle_length;
double triangle_length_increment = 0.1;

// Sphere
double max_sphere_radius = max_triangle_length / sqrt(3.0);
double sphere_radius = 0;
double sphere_radius_increment =
    max_sphere_radius / (max_triangle_length / triangle_length_increment);

// Axis Rotation
double axis_rotation_angle =
    45;  // in degrees, for rotation of the object on its own axis
double axis_angle_increment = 5;  // in degrees

// Function Declarations
void init();
void draw_line(const Point3D& a, const Point3D& b);
void draw_axes();
void draw_triangle(const Point3D& a, const Point3D& b, const Point3D& c);
void draw_sphere_quads(double radius, int stack_count, int sector_count);
void draw_spheres();
void draw_octahedron();
void draw_single_cylinder(double height, double radius, int segment_count);
void draw_cylinders();
void display();
void idle();
void handle_keys(unsigned char key, int x, int y);
void handle_special_keys(int key, int x, int y);

void init() {
  camera.rotation_speed = 1;
  camera.speed = 0.5;
  glClearColor(0.0f, 0.0f, 0.0f,
               1.0f);  // Set background color to black and opaque

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(80, 1, 1, 1000.0);
}

void draw_line(const Point3D& a, const Point3D& b) {
  glBegin(GL_LINES);
  {
    glVertex3f(a.x, a.y, a.z);
    glVertex3f(b.x, b.y, b.z);
  }
  glEnd();
}

void draw_axes() {
  glLineWidth(3);
  glColor3f(1, 0, 0);
  draw_line(Point3D(0, 0, 0), Point3D(5, 0, 0));
  glColor3f(0, 1, 0);
  draw_line(Point3D(0, 0, 0), Point3D(0, 5, 0));
  glColor3f(0, 0, 1);
  draw_line(Point3D(0, 0, 0), Point3D(0, 0, 5));
  glLineWidth(1);
}

void draw_triangle(const Point3D& a, const Point3D& b, const Point3D& c) {
  glBegin(GL_TRIANGLES);
  {
    glVertex3f(a.x, a.y, a.z);
    glVertex3f(b.x, b.y, b.z);
    glVertex3f(c.x, c.y, c.z);
  }
  glEnd();
}

void draw_sphere_quads(double radius, int stack_count, int sector_count) {
  std::vector<std::vector<Point3D>> points(
      stack_count + 1, std::vector<Point3D>(sector_count + 1));

  // Generate Points
  for (int i = 0; i <= stack_count; i++) {
    for (int j = 0; j <= sector_count; j++) {
      points[i][j].x = -1 + 2.0 * i / stack_count;
      points[i][j].y = -1 + 2.0 * j / sector_count;
      // taking z = 1 only to get the top quad
      points[i][j].z = 1;

      points[i][j] *= radius / sqrt(points[i][j].x * points[i][j].x +
                                    points[i][j].y * points[i][j].y +
                                    points[i][j].z * points[i][j].z);
    }
  }
  // Draw Quads
  for (int i = 0; i < stack_count; i++) {
    for (int j = 0; j < sector_count; j++) {
      glBegin(GL_QUADS);
      {
        glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
        glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
        glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y,
                   points[i + 1][j + 1].z);
        glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);
      }
      glEnd();
    }
  }
}

void draw_spheres() {
  // 4 sphers along x and z axis
  for (int i = 0; i < 4; i++) {
    glPushMatrix();
    {
      // blue or green depending on i
      if (i % 2)
        glColor3f(0, 0, 1);
      else
        glColor3f(0, 1, 0);
      glRotatef(i * 90, 0, 1, 0);
      glTranslatef(0, 0, triangle_length);
      draw_sphere_quads(sphere_radius, 100, 100);
    }
    glPopMatrix();
  }
  // the two spheres along the y axis
  for (int i = 0; i < 2; i++) {
    glPushMatrix();
    {
      glColor3f(1, 0, 0);  // red
      glRotatef(90 + 180 * i, 1, 0, 0);
      glTranslatef(0, 0, triangle_length);
      draw_sphere_quads(sphere_radius, 100, 100);
    }
    glPopMatrix();
  }
}

void draw_octahedron() {
  double diff = (max_triangle_length - triangle_length) / 3.0;

  // 0 to 3: 4 pyramids to the side of +ve y axis
  // 4 to 7: 4 pyramids to the side of -ve y axis
  for (int i = 0; i < 8; i++) {
    int idx = i % 4;
    glPushMatrix();
    {
      if (i < 4) 
        glColor3f(1 - (i % 2), i % 2, 1);
      else 
        glColor3f(i % 2, 1 - (i % 2), 1);
      glRotatef(idx * 90, 0, 1, 0);
      if (i >= 4) glRotatef(180, 1, 0, 1); // for rotating upside down
      glTranslatef(diff, diff, diff);
      glScaled(triangle_length, triangle_length, triangle_length);
      draw_triangle(Point3D(1, 0, 0), Point3D(0, 1, 0), Point3D(0, 0, 1));
    }
    glPopMatrix();
  }
}

void draw_single_cylinder(double height, double radius, int segment_count) {
  std::vector<Point3D> points(segment_count + 1);

  const double dihedral_angle = acos(-1.0 / 3.0);
  double offset = M_PI - dihedral_angle;

  for (int i = 0; i <= segment_count; i++) {
    // Point Generation
    double angle = -offset / 2.0 + i * offset / segment_count;
    points[i] = Point3D(radius * cos(angle), radius * sin(angle), 0);

    // drawing the cylinder on the go
    if (i) {
      glBegin(GL_QUADS);
      {
        glVertex3f(points[i - 1].x, points[i - 1].y, -height / 2.0);
        glVertex3f(points[i - 1].x, points[i - 1].y, height / 2.0);
        glVertex3f(points[i].x, points[i].y, height / 2.0);
        glVertex3f(points[i].x, points[i].y, -height / 2.0);
      }
      glEnd();
    }
  }
}

void draw_cylinders() {
  glColor3f(1, 1, 0);  // all yellow

  // First 4: Aligned with x axis
  // Next 4: Middle 4 cylinders (horizontal)
  // Last 4: Aligned with y axis
  for (int i = 0; i < 12; i++) {
    int idx = i % 4;
    glPushMatrix();
    {
      if (i >= 4 && i < 8)
        glRotatef(90, 1, 0, 0);
      else if (i >= 8)
        glRotatef(90, 0, 0, 1);
      glRotatef(45 + idx * 90, 0, 1, 0);
      glTranslatef(triangle_length / sqrt(2.0), 0, 0);
      draw_single_cylinder(triangle_length * sqrt(2.0), sphere_radius, 100);
    }
    glPopMatrix();
  }
}

void display() {
  glEnable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(camera.pos.x, camera.pos.y, camera.pos.z,
            camera.pos.x + camera.look.x, camera.pos.y + camera.look.y,
            camera.pos.z + camera.look.z, camera.up.x, camera.up.y,
            camera.up.z);

  glRotatef(axis_rotation_angle, 0, 0, 1);
  // draw_axes();
  draw_octahedron();
  draw_spheres();
  draw_cylinders();

  glutSwapBuffers();
}

void idle() {
  // printf("Camera: (%lf, %lf, %lf)\n", camera.pos.x, camera.pos.y,
  // camera.pos.z);
  glutPostRedisplay();
}

void handle_keys(unsigned char key, int x, int y) {
  switch (key) {
    case '1':
      camera.look_left();
      break;
    case '2':
      camera.look_right();
      break;
    case '3':
      camera.look_up();
      break;
    case '4':
      camera.look_down();
      break;
    case '5':
      camera.tilt_clockwise();
      break;
    case '6':
      camera.tilt_counterclockwise();
      break;
    case 'w':
      camera.move_up_same_ref();
      break;
    case 's':
      camera.move_down_same_ref();
      break;
    case ',':
      triangle_length -= triangle_length_increment;
      sphere_radius += sphere_radius_increment;
      if (triangle_length <= 0) {
        triangle_length = 0;
        sphere_radius = max_sphere_radius;
      }
      break;
    case '.':
      triangle_length += triangle_length_increment;
      sphere_radius -= sphere_radius_increment;
      if (triangle_length >= max_triangle_length) {
        triangle_length = max_triangle_length;
        sphere_radius = 0;
      }
      break;
    case 'a':
      axis_rotation_angle += axis_angle_increment;
      while (axis_rotation_angle >= 360) axis_rotation_angle -= 360;
      break;
    case 'd':
      axis_rotation_angle -= axis_angle_increment;
      while (axis_rotation_angle < 0) axis_rotation_angle += 360;
      break;
    default:
      printf("Unknown key pressed\n");
      break;
  }
}

void handle_special_keys(int key, int x, int y) {
  switch (key) {
    case GLUT_KEY_UP:
      camera.move_forward();
      break;
    case GLUT_KEY_DOWN:
      camera.move_backward();
      break;
    case GLUT_KEY_LEFT:
      camera.move_left();
      break;
    case GLUT_KEY_RIGHT:
      camera.move_right();
      break;
    case GLUT_KEY_PAGE_UP:
      camera.move_up();
      break;
    case GLUT_KEY_PAGE_DOWN:
      camera.move_down();
      break;
    default:
      printf("Unknown key pressed\n");
      break;
  }
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitWindowSize(600, 600);
  glutInitWindowPosition(100, 100);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow("Magic Cube");
  glutDisplayFunc(display);
  glutKeyboardFunc(handle_keys);
  glutSpecialFunc(handle_special_keys);
  glutIdleFunc(idle);
  init();

  glutMainLoop();

  return 0;
}