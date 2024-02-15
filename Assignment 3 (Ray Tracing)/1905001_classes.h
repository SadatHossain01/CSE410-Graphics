#ifndef CLASSES_H
#define CLASSES_H

#include <GL/glut.h>
#include <bits/stdc++.h>

// Forward Declarations
struct Vector;
struct Camera;
class Object;
class Sphere;
class Triangle;
class GeneralQuadraticSurface;
class Floor;
class PointLight;
class SpotLight;

struct Vector {
   public:
    double x, y, z;
    Vector(double x = 0, double y = 0, double z = 0);
    Vector operator+(const Vector& v) const;
    Vector operator+=(const Vector& v);
    Vector operator-(const Vector& v) const;
    Vector operator-=(const Vector& v);
    Vector operator*(const double& d) const;
    friend Vector operator*(const double& d, const Vector& v);
    Vector operator*=(const double& d);
    Vector operator/(const double& d) const;
    Vector operator/=(const double& d);
    double dot(const Vector& v) const;
    Vector cross(const Vector& v) const;
    Vector normalize() const;
    // angle in degrees; normal should be normalized
    Vector rotate(const Vector& axis, double angle) const;
    bool check_normalized() const;
    bool check_orthogonal(const Vector& v) const;
    double norm() const;

    friend double distance(const Vector& a, const Vector& b);
    friend std::istream& operator>>(std::istream& is, Vector& v);
    friend std::ostream& operator<<(std::ostream& os, const Vector& v);
};

struct Camera {
   public:
    double speed;            // for movement operations
    double rotation_speed;   // in degrees
    Vector pos;
    Vector up, right, look;  // normalized vectors

    Camera(const Vector& pos = Vector(5, 5, 5),
           const Vector& obj = Vector(0, 0, 0),
           const Vector& up = Vector(0, 0, 1), double speed = 2,
           double rotation_speed = 0.5);
    void move_forward();
    void move_backward();
    void move_left();
    void move_right();
    void move_up();
    void move_down();
    // Camera rotation functions
    void look_left();
    void look_right();
    void look_up();
    void look_down();
    void tilt_clockwise();
    void tilt_counterclockwise();

    void move_up_same_ref();
    void move_down_same_ref();
};

class Object {
   protected:
    Vector reference_point;
    double color[3];      // red, green, blue
    double
        coefficients[4];  // ambient, diffuse, specular, reflection coefficients
    int shine;            // exponent term of specular component

   public:
    Object(const Vector& ref = Vector(0, 0, 0));
    ~Object();
    virtual void draw() {}
    void set_color(double r, double g, double b);
    void set_shine(int shine);
    void set_coefficients(double ambient, double diffuse, double specular,
                          double reflection);
};

class Floor : public Object {
   private:
    double floor_width, tile_width;

   public:
    Floor(double floor_width, double tile_width);
    void draw();
};

class Sphere : public Object {
   protected:
    double radius;

   public:
    Sphere(const Vector& center, double radius);
    void draw();
};

class Triangle : public Object {
   private:
    Vector a, b, c;

   public:
    Triangle(const Vector& a, const Vector& b, const Vector& c);
    void draw();
};

class GeneralQuadraticSurface : public Object {
   private:
    double A, B, C, D, E, F, G, H, I, J;
    double length, width, height;

   public:
    GeneralQuadraticSurface(double A, double B, double C, double D, double E,
                            double F, double G, double H, double I, double J,
                            const Vector& ref, double l, double w, double h);
    void draw();
};

class PointLight {
   protected:
    Vector light_position;  // position of the light source
    double color[3];        // red, green, blue

   public:
    PointLight(const Vector& pos, double r, double g, double b);
    ~PointLight();
};

class SpotLight {
   protected:
    PointLight point_light;
    Vector light_direction;  // direction of the light source
    double cutoff_angle;     // in degrees

   public:
    SpotLight(const Vector& pos, double r, double g, double b,
              const Vector& dir, double angle);
    ~SpotLight();
};

// Function Prototypes
void draw_line(const Vector& a, const Vector& b);
void draw_triangle(const Vector& a, const Vector& b, const Vector& c);
void draw_quad(const Vector& a, const Vector& b, const Vector& c,
               const Vector& d);
void draw_sphere(double radius, int stacks, int sectors);

#endif