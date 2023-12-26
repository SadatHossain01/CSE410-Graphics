#include <bits/stdc++.h>

#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <GL/glut.h>
#include <windows.h>
#endif

#include "ball.h"
#include "camera.h"

Camera camera(Point3D(80, 80, 115), Point3D(0, 0, 0), Vector(0, 0, 1));
Ball ball;
bool simulation_on = false;
int n_walls;
const double EPS = 1e-6;

// Wall Constants
const double wall_height = 10;
const double wall_thickness = 2;
const double wall_width = 100;
std::vector<Point3D> box_vertices;

// Function Declarations
void init();
void draw_axes();
void generate_wall_vertices(int n_walls);
void draw_square(double a);
void draw_wall(const Point3D &a, const Point3D &b, const Point3D &c,
               const Point3D &d);
void draw_box(double r, double g, double b);
void draw_checkerboard();
void draw_line(const Point3D &a, const Point3D &b);
void draw_triangle(const Point3D &a, const Point3D &b, const Point3D &c);
void draw_arrow(const Point3D &s, const Vector &dir, const Vector &normal,
                double len);
void draw_sphere(double radius, int stack_count, int sector_count,
                 const std::vector<Point3D> &points);
void display();
void idle();
void handle_simulation(int value);
void handle_keys(unsigned char key, int x, int y);
void handle_special_keys(int key, int x, int y);

void init() {
  glClearColor(0.0f, 0.0f, 0.0f,
               1.0f);  // Set background color to black and opaque

  generate_wall_vertices(n_walls);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(80, 1, 1, 1000.0);
}

void generate_wall_vertices(int n) {
  if (n < 3) n = 4;
  if (n == 4) {
    box_vertices.push_back(
        Point3D(-wall_width / 2.0, -wall_width / 2.0, ball.radius));
    box_vertices.push_back(
        Point3D(-wall_width / 2.0, wall_width / 2.0, ball.radius));
    box_vertices.push_back(
        Point3D(wall_width / 2.0, wall_width / 2.0, ball.radius));
    box_vertices.push_back(
        Point3D(wall_width / 2.0, -wall_width / 2.0, ball.radius));
    box_vertices.push_back(box_vertices.front());  // close the wall
  } else {
    double angle = 360.0 / n;
    Vector vec = Vector(100, 0, ball.radius);
    for (int i = 0; i < n; i++) {
      box_vertices.push_back(Point3D(vec.x, vec.y, vec.z));
      vec = vec.rotate(Vector(0, 0, 1), angle);
    }
    box_vertices.push_back(box_vertices.front());  // close the wall
  }

  ball.box_vertices = box_vertices;
}

void draw_axes() {
  glLineWidth(3);
  glColor3f(1, 0, 0);
  draw_line(Point3D(0, 0, 0), Point3D(100, 0, 0));
  glColor3f(0, 1, 0);
  draw_line(Point3D(0, 0, 0), Point3D(0, 100, 0));
  glColor3f(0, 0, 1);
  draw_line(Point3D(0, 0, 0), Point3D(0, 0, 100));
  glLineWidth(1);
}

void draw_square(double a) {
  glBegin(GL_QUADS);
  {
    glVertex3f(a, a, 0);
    glVertex3f(-a, a, 0);
    glVertex3f(-a, -a, 0);
    glVertex3f(a, -a, 0);
  }
  glEnd();
}

// the points are in ccw order
void draw_wall(const Point3D &a, const Point3D &b, const Point3D &c,
               const Point3D &d) {
  glBegin(GL_QUADS);
  {
    glVertex3f(a.x, a.y, a.z);
    glVertex3f(b.x, b.y, b.z);
    glVertex3f(c.x, c.y, c.z);
    glVertex3f(d.x, d.y, d.z);
  }
  glEnd();
}

void draw_box(double r, double g, double b) {
  glColor3f(r, g, b);
  for (int i = 0; i < box_vertices.size() - 1; i++) {
    Point3D aa = box_vertices[i] + Point3D(0, 0, -wall_height / 2.0);
    Point3D bb = box_vertices[i + 1] + Point3D(0, 0, -wall_height / 2.0);

    Point3D a, b;  // another set of points for the outer layer)
    a = aa, b = bb;
    a.x += (a.x > 0 ? 1 : -1) * wall_thickness;
    a.y += (a.y > 0 ? 1 : -1) * wall_thickness;
    b.x += (b.x > 0 ? 1 : -1) * wall_thickness;
    b.y += (b.y > 0 ? 1 : -1) * wall_thickness;

    draw_wall(aa, bb, bb + Point3D(0, 0, wall_height),
              aa + Point3D(0, 0, wall_height));
    draw_wall(a, b, b + Point3D(0, 0, wall_height),
              a + Point3D(0, 0, wall_height));
    draw_wall(aa, a, a + Point3D(0, 0, wall_height),
              aa + Point3D(0, 0, wall_height));
    draw_wall(bb, b, b + Point3D(0, 0, wall_height),
              bb + Point3D(0, 0, wall_height));
    draw_wall(aa, bb, b, a);
    draw_wall(aa + Point3D(0, 0, wall_height), bb + Point3D(0, 0, wall_height),
              b + Point3D(0, 0, wall_height), a + Point3D(0, 0, wall_height));
  }
}

void draw_checkerboard() {
  double divisions = 100;
  double size = 8;

  for (int i = -divisions; i < divisions; i++) {
    for (int j = -divisions; j < divisions; j++) {
      glPushMatrix();
      {
        glTranslatef(i * size, j * size, 0);
        bool is_white = (i + j) % 2 == 0;
        glColor3f(is_white, is_white, is_white);
        draw_square(size / 2);
      }
      glPopMatrix();
    }
  }
}

void draw_line(const Point3D &a, const Point3D &b) {
  glBegin(GL_LINES);
  {
    glVertex3f(a.x, a.y, a.z);
    glVertex3f(b.x, b.y, b.z);
  }
  glEnd();
}

void draw_triangle(const Point3D &a, const Point3D &b, const Point3D &c) {
  glBegin(GL_TRIANGLES);
  {
    glVertex3f(a.x, a.y, a.z);
    glVertex3f(b.x, b.y, b.z);
    glVertex3f(c.x, c.y, c.z);
  }
  glEnd();
}

/**
 * @brief Draw an arrow with the given properties.
 * @param s The starting point of the arrow.
 * @param dir The direction vector of the arrow.
 * @param normal The normal vector of the arrow.
 * @param len The length of the arrow.
 * @param r The red component of the arrow color.
 * @param g The green component of the arrow color.
 * @param b The blue component of the arrow color.
 *
 * @note The normal vector is necessary to draw the arrowhead in the proper
 * plane.
 */
void draw_arrow(const Point3D &s, const Vector &dir, const Vector &normal,
                double len) {
  Vector new_dir = dir.normalize();
  Vector new_normal = normal.normalize();

  std::vector<Point3D> points;
  Vector right = new_dir.cross(new_normal);

  const double bottom_width = 0.5;
  const double head_width = 1.5;
  const double head_length = 3;

  // bottom quad points
  points.push_back(s - (bottom_width / 2.0) * right);      // bottom left
  points.push_back(points.back() + bottom_width * right);  // bottom right
  points.push_back(points.back() + len * new_dir);
  points.push_back(points.back() - bottom_width * right);
  Point3D temp = points.back();
  glBegin(GL_QUADS);
  {
    for (auto p : points) glVertex3f(p.x, p.y, p.z);
  }
  glEnd();
  points.clear();

  // upper arrowhead points
  points.push_back(temp - (head_width - bottom_width) / 2.0 * right);
  points.push_back(points.back() + head_width * right);
  points.push_back(s + (len + head_length) * dir);  // topmost point
  glBegin(GL_TRIANGLES);
  {
    for (auto p : points) glVertex3f(p.x, p.y, p.z);
  }
  glEnd();
}

void draw_sphere(double radius, int stack_count, int sector_count,
                 const std::vector<Point3D> &points) {
  double c1r = 1, c1g = 0.22, c1b = 0.22;
  double c2r = 0, c2g = 1, c2b = 0.22;

  for (int i = 0; i < stack_count; i++) {
    int k1 = i * (sector_count + 1);
    int k2 = k1 + sector_count + 1;
    bool color1 = (i < stack_count / 2);
    for (int j = 0; j < sector_count; j++) {
      glColor3f(color1 ? c1r : c2r, color1 ? c1g : c2g, color1 ? c1b : c2b);
      if (i != 0)
        draw_triangle(points[k1 + j], points[k2 + j], points[k1 + 1 + j]);
      if (i != stack_count - 1)
        draw_triangle(points[k1 + 1 + j], points[k2 + j], points[k2 + 1 + j]);

      draw_line(points[j + k1], points[j + k2]);
      if (i != 0) draw_line(points[j + k1], points[j + k1 + 1]);
      color1 ^= 1;
    }
  }
}

unsigned int up_counter = 0;
void display() {
  glEnable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(camera.pos.x, camera.pos.y, camera.pos.z,
            camera.pos.x + camera.look.x, camera.pos.y + camera.look.y,
            camera.pos.z + camera.look.z, camera.up.x, camera.up.y,
            camera.up.z);

  // draw_axes();
  draw_checkerboard();
  draw_box(1, 0, 0);
  glPushMatrix();
  {
    glTranslatef(ball.center.x, ball.center.y, ball.center.z);
    draw_sphere(ball.radius, ball.stack_count, ball.sector_count,
                ball.ball_vertices);
    glPopMatrix();
  }
  if (up_counter < 7000) {
    glColor3f(0, 1, 1);
    draw_arrow(ball.center, ball.up, Vector(1, 1, 0), ball.radius + 6);
  }
  glColor3f(0, 0, 1);
  draw_arrow(ball.center, ball.dir, ball.up, ball.radius + 3);
  glutSwapBuffers();
  up_counter++;
  if (up_counter == 14000) up_counter = 0;
}

std::chrono::steady_clock::time_point last_time =
    std::chrono::steady_clock::now();

void handle_simulation(int value) {
  if (!simulation_on) return;
  if (value == 1) {
    std::chrono::steady_clock::time_point current_time =
        std::chrono::steady_clock::now();
    double time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                              current_time - last_time)
                              .count();
    if (time_elapsed < ball.dt) {
      glutTimerFunc(ball.dt - time_elapsed, handle_simulation, 1);
      return;
    }
    last_time = current_time;
    // typical ball movement
    ball.center += ball.dir_scalar_multiplier * ball.dir;
    double angle = 360 * (ball.dir.norm() * ball.dir_scalar_multiplier) /
                   (2 * M_PI * ball.radius);
    ball.rotate_ball_vertices(ball.right, -angle);
    glutTimerFunc(ball.dt, handle_simulation, 1);
  } else if (value == 2) {
    Point3D temp = ball.center + ball.dir_scalar_multiplier * ball.dir;
    if (!ball.is_ball_inside_box(temp, ball.radius, box_vertices))
      ball.update_direction_after_collision();
    double time = ball.next_collision_time();
    glutTimerFunc(time, handle_simulation, 2);
  }
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
    case 'i':
      if (!simulation_on) ball.go_forward();
      break;
    case 'k':
      if (!simulation_on) ball.go_backward();
      break;
    case 'j':
      ball.rotate_dir_ccw();
      if (simulation_on) {
        double time = ball.next_collision_time();
        glutTimerFunc(time, handle_simulation, 2);
      }
      break;
    case 'l':
      ball.rotate_dir_cw();
      if (simulation_on) {
        double time = ball.next_collision_time();
        glutTimerFunc(time, handle_simulation, 2);
      }
      break;
    case ' ':
      simulation_on ^= 1;
      if (simulation_on) {
        double time = ball.next_collision_time();
        glutTimerFunc(ball.dt, handle_simulation, 1);
        glutTimerFunc(time, handle_simulation, 2);
      }
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

int main(int argc, char **argv) {
  n_walls = argc > 1 ? atoi(argv[1]) : 4;
  glutInit(&argc, argv);
  glutInitWindowSize(480, 480);
  glutInitWindowPosition(100, 100);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow("Rolling Ball");
  glutDisplayFunc(display);
  glutKeyboardFunc(handle_keys);
  glutSpecialFunc(handle_special_keys);
  glutIdleFunc(idle);
  init();

  glutMainLoop();

  return 0;
}