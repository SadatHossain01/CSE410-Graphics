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

void init() {
  glClearColor(0.0f, 0.0f, 0.0f,
               1.0f);  // Set background color to black and opaque
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

void draw_sphere(const Ball& ball) {
  for (int i = 0; i < ball.stack_count; i++) {
    int k1 = i * (ball.sector_count + 1);
    int k2 = k1 + ball.sector_count + 1;
    for (int j = 0; j < ball.sector_count; j++) {
      if (j % 2)
        glColor3f(1, 0, 0);
      else
        glColor3f(0, 1, 0);
      if (i != 0)
        draw_triangle(ball.vertices[k1 + j], ball.vertices[k2 + j],
                      ball.vertices[k1 + 1 + j]);
      if (i != ball.stack_count - 1)
        draw_triangle(ball.vertices[k1 + 1 + j], ball.vertices[k2 + j],
                      ball.vertices[k2 + 1 + j]);

      draw_line(ball.vertices[j + k1], ball.vertices[j + k2]);
      if (i != 0) draw_line(ball.vertices[j + k1], ball.vertices[j + k1 + 1]);
    }
  }

  // now draw the direction vector
  // glColor3f(0, 0, 1);
  // draw_line(Point3D(0, 0, 0), Point3D(10 * ball.radius * ball.dir.x,
  //                                     10 * ball.radius * ball.dir.y,
  //                                     10 * ball.radius * ball.dir.z));
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
  glPushMatrix();
  glTranslatef(0, 0, 5);
  draw_sphere(ball);
  glPopMatrix();
  glutSwapBuffers();
}

void idle() {
  // printf("Camera position: (%lf, %lf, %lf)\n", camera.pos.x, camera.pos.y,
  //        camera.pos.z);
  // printf("Camera look: (%lf, %lf, %lf)\n", camera.look.x, camera.look.y,
  //        camera.look.z);
  // printf("Camera up: (%lf, %lf, %lf)\n", camera.up.x, camera.up.y,
  // camera.up.z); printf("Camera right: (%lf, %lf, %lf)\n", camera.right.x,
  // camera.right.y,
  //        camera.right.z);
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