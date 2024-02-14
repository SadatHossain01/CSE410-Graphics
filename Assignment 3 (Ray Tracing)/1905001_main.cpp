#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <GL/glut.h>
#include <windows.h>
#endif

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "1905001_classes.h"

int reflection_depth;
int screen_width, screen_height;

Camera camera;
std::vector<Object *> objects;
std::vector<PointLight *> point_lights;
std::vector<SpotLight *> spot_lights;

// Function Declarations
void init();
void display();
void idle();
void handle_keys(unsigned char key, int x, int y);
void handle_special_keys(int key, int x, int y);
void load_data(const std::string &filename);
void free_memory();

void init() {
    glClearColor(0.0f, 0.0f, 0.0f,
                 1.0f);  // Set background color to black and opaque

    load_data("scene.txt");

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(80, 1, 1, 1000.0);
}

void free_memory() {
    for (auto &object : objects) delete object;
    for (auto &point_light : point_lights) delete point_light;
    for (auto &spot_light : spot_lights) delete spot_light;
}

void load_data(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: File not found" << std::endl;
        return;
    }

    int pixel;
    file >> reflection_depth >> pixel;
    screen_width = screen_height = pixel;

    int num_objects;
    file >> num_objects;

    for (int i = 0; i < num_objects; i++) {
        std::string type;
        file >> type;

        if (type == "sphere") {
            double x, y, z, radius;
            file >> x >> y >> z >> radius;
            double r, g, b;
            file >> r >> g >> b;
            double ambient, diffuse, specular, reflection;
            file >> ambient >> diffuse >> specular >> reflection;
            int shine;
            file >> shine;
            Vector center(x, y, z);
            Object *temp = new Sphere(center, radius);
            temp->set_color(r, g, b);
            temp->set_coefficients(ambient, diffuse, specular, reflection);
            temp->set_shine(shine);
            objects.push_back(temp);
        } else if (type == "triangle") {
            double x1, y1, z1, x2, y2, z2, x3, y3, z3;
            file >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> x3 >> y3 >> z3;
            Vector p1(x1, y1, z1), p2(x2, y2, z2), p3(x3, y3, z3);
            double r, g, b;
            file >> r >> g >> b;
            double ambient, diffuse, specular, reflection;
            file >> ambient >> diffuse >> specular >> reflection;
            int shine;
            file >> shine;
            Object *temp = new Triangle(p1, p2, p3);
            temp->set_color(r, g, b);
            temp->set_coefficients(ambient, diffuse, specular, reflection);
            temp->set_shine(shine);
            objects.push_back(temp);
        } else if (type == "general") {
            double A, B, C, D, E, F, G, H, I, J;
            file >> A >> B >> C >> D >> E >> F >> G >> H >> I >> J;
            double x, y, z;
            file >> x >> y >> z;
            double length, width, height;
            file >> length >> width >> height;
            double r, g, b;
            file >> r >> g >> b;
            double ambient, diffuse, specular, reflection;
            file >> ambient >> diffuse >> specular >> reflection;
            int shine;
            file >> shine;
            Vector reference_point(x, y, z);
            Object *temp = new GeneralQuadraticSurface(A, B, C, D, E, F, G, H,
                                                       I, J, reference_point,
                                                       length, width, height);
            temp->set_color(r, g, b);
            temp->set_coefficients(ambient, diffuse, specular, reflection);
            temp->set_shine(shine);
            objects.push_back(temp);
        } else {
            std::cerr << "Error reading file: Unknown object type" << std::endl;
            return;
        }
    }

    // The Floor
    Object *floor = new Floor(1000, 20);
    // set color
    // set coefficients
    // set shine
    objects.push_back(floor);

    // Point Light Sources
    int num_point_lights;
    file >> num_point_lights;
    for (int i = 0; i < num_point_lights; i++) {
        double x, y, z;
        file >> x >> y >> z;
        double r, g, b;
        file >> r >> g >> b;
        Vector position(x, y, z);
        PointLight *pl = new PointLight(position, r, g, b);
        point_lights.push_back(pl);
    }

    // Spot Light Sources
    int num_spot_lights;
    file >> num_spot_lights;
    for (int i = 0; i < num_spot_lights; i++) {
        double x, y, z;
        file >> x >> y >> z;
        double r, g, b;
        file >> r >> g >> b;
        Vector position(x, y, z);
        double direction_x, direction_y, direction_z;
        file >> direction_x >> direction_y >> direction_z;
        double angle;
        file >> angle;
        Vector direction(direction_x, direction_y, direction_z);
        SpotLight *sl = new SpotLight(position, r, g, b, direction, angle);
        spot_lights.push_back(sl);
    }

    file.close();
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

    glutSwapBuffers();
}

void idle() { glutPostRedisplay(); }

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

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(screen_width, screen_height);
    glutInitWindowPosition(100, 100);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Ray Tracing");
    glutDisplayFunc(display);
    glutKeyboardFunc(handle_keys);
    glutSpecialFunc(handle_special_keys);
    glutIdleFunc(idle);
    init();

    glutMainLoop();

    free_memory();

    return 0;
}