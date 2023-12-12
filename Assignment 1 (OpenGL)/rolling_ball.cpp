#include <bits/stdc++.h>

#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <glut.h>
#include <windows.h>
#endif

#include "ball.h"
#include "camera.h"

Camera camera;
Ball ball;
bool simulation_on = false;

// Wall Constants
const double wall_height = 5;
std::vector<Point3D> box_vertices;

void init() {
  glClearColor(0.0f, 0.0f, 0.0f,
               1.0f);  // Set background color to black and opaque

  const double wall_width = 200;
  box_vertices.push_back(Point3D(-wall_width / 2.0, -wall_width / 2.0, 0));
  box_vertices.push_back(Point3D(-wall_width / 2.0, wall_width / 2.0, 0));
  box_vertices.push_back(Point3D(wall_width / 2.0, wall_width / 2.0, 0));
  box_vertices.push_back(Point3D(wall_width / 2.0, -wall_width / 2.0, 0));
  box_vertices.push_back(box_vertices.front());  // close the wall

  ball.box_vertices = box_vertices;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(80, 1, 1, 1000.0);
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

void draw_wall(double r, double g, double b) {
  glColor3f(r, g, b);
  for (int i = 0; i < box_vertices.size() - 1; i++) {
    glBegin(GL_QUADS);
    {
      glVertex3f(box_vertices[i].x, box_vertices[i].y, box_vertices[i].z);
      glVertex3f(box_vertices[i + 1].x, box_vertices[i + 1].y,
                 box_vertices[i + 1].z);
      glVertex3f(box_vertices[i + 1].x, box_vertices[i + 1].y,
                 box_vertices[i + 1].z + wall_height);
      glVertex3f(box_vertices[i].x, box_vertices[i].y,
                 box_vertices[i].z + wall_height);
    }
    glEnd();
  }
}

void draw_checkerboard() {
  double divisions = 100;
  double size = 8;

  for (int i = -divisions; i < divisions; i++) {
    for (int j = -divisions; j < divisions; j++) {
      glPushMatrix();
      glTranslatef(i * size, j * size, 0);
      bool is_white = (i + j) % 2 == 0;
      glColor3f(is_white, is_white, is_white);
      draw_square(size / 2);
      glPopMatrix();
    }
  }
}

void draw_line(Point3D a, Point3D b) {
  glBegin(GL_LINES);
  {
    glVertex3f(a.x, a.y, a.z);
    glVertex3f(b.x, b.y, b.z);
  }
  glEnd();
}

void draw_triangle(Point3D a, Point3D b, Point3D c) {
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
void draw_arrow(Point3D s, Vector dir, Vector normal, double len, double r,
                double g, double b) {
  glColor3f(r, g, b);
  dir = dir.normalize();
  normal = normal.normalize();

  std::vector<Point3D> points;
  Vector right = dir.cross(normal);

  const double bottom_width = 0.5;
  const double head_width = 1.5;
  const double head_length = 3;

  // bottom quad points
  points.push_back(s - (bottom_width / 2.0) * right);      // bottom left
  points.push_back(points.back() + bottom_width * right);  // bottom right
  points.push_back(points.back() + len * dir);
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

void draw_sphere(const Ball& ball) {
  double c1r = 1, c1g = 0.22, c1b = 0.22;
  double c2r = 0, c2g = 1, c2b = 0.22;

  for (int i = 0; i < ball.stack_count; i++) {
    int k1 = i * (ball.sector_count + 1);
    int k2 = k1 + ball.sector_count + 1;
    bool color1 = (i < ball.stack_count / 2);
    for (int j = 0; j < ball.sector_count; j++) {
      glColor3f(color1 ? c1r : c2r, color1 ? c1g : c2g, color1 ? c1b : c2b);
      if (i != 0)
        draw_triangle(ball.ball_vertices[k1 + j], ball.ball_vertices[k2 + j],
                      ball.ball_vertices[k1 + 1 + j]);
      if (i != ball.stack_count - 1)
        draw_triangle(ball.ball_vertices[k1 + 1 + j],
                      ball.ball_vertices[k2 + j],
                      ball.ball_vertices[k2 + 1 + j]);

      draw_line(ball.ball_vertices[j + k1], ball.ball_vertices[j + k2]);
      if (i != 0)
        draw_line(ball.ball_vertices[j + k1], ball.ball_vertices[j + k1 + 1]);
      color1 ^= 1;
    }
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

  draw_checkerboard();
  draw_wall(1, 0, 0);
  glPushMatrix();
  glTranslatef(ball.center.x, ball.center.y, ball.center.z);
  draw_sphere(ball);
  glPopMatrix();
  draw_arrow(ball.center, ball.dir, ball.up, ball.radius + 3, 0, 0, 1);
  glutSwapBuffers();
}

void handle_simulation(int value) {
  if (!simulation_on) return;
  ball.go_forward();
  glutTimerFunc(ball.dt, handle_simulation, 0);
}

void handle_collision(int value) {}

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
      ball.go_forward();
      break;
    case 'k':
      ball.go_backward();
      break;
    case 'j':
      ball.rotate_dir_ccw();
      break;
    case 'l':
      ball.rotate_dir_cw();
      break;
    case ' ':
      simulation_on ^= 1;
      if (simulation_on) glutTimerFunc(ball.dt, handle_simulation, 0);
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