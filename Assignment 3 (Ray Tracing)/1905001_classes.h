#ifndef CLASSES_H
#define CLASSES_H

#include <iostream>

// Forward Declarations
class Vector;
class Camera;
class Object;
class Sphere;
class Triangle;
class GeneralQuadraticSurface;
class Floor;
class PointLight;
class SpotLight;

class Vector {
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

class Camera {
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
   private:
    Vector reference_point;
    double color[3];      // red, green, blue
    double
        coefficients[4];  // ambient, diffuse, specular, reflection coefficients
    int shine;            // exponent term of specular component

   public:
    Object(const Vector& ref = Vector(0, 0, 0)) : reference_point(ref) {}
    ~Object();
    virtual void draw() {}
    void set_color(double r, double g, double b) {}
    void set_shine(int shine) {}
    void set_coefficients(double ambient, double diffuse, double specular,
                          double reflection) {}
};

class Floor : public Object {
   private:
    double floor_width, tile_width;

   public:
    Floor(double floor_width, double tile_width)
        : Object(Vector(-floor_width / 2.0, -floor_width / 2.0, 0.0)),
          floor_width(floor_width),
          tile_width(tile_width) {}
    void draw();
};

class Sphere : public Object {
   private:
    double radius;

   public:
    Sphere(const Vector& center, double radius)
        : Object(center), radius(radius) {}
    void draw();
};

class Triangle : public Object {
   private:
    Vector a, b, c;

   public:
    Triangle(const Vector& a, const Vector& b, const Vector& c)
        : a(a), b(b), c(c) {}
    void draw();
};

class GeneralQuadraticSurface : public Object {
   private:
    double A, B, C, D, E, F, G, H, I, J;
    double length, width, height;

   public:
    GeneralQuadraticSurface(double A, double B, double C, double D, double E,
                            double F, double G, double H, double I, double J,
                            const Vector& ref, double l, double w, double h)
        : Object(ref),
          A(A),
          B(B),
          C(C),
          D(D),
          E(E),
          F(F),
          G(G),
          H(H),
          I(I),
          J(J),
          length(l),
          width(w),
          height(h) {}
    void draw();
};

class PointLight {
   private:
    Vector light_position;  // position of the light source
    double color[3];        // red, green, blue
   public:
    PointLight(const Vector& pos, double r, double g, double b)
        : light_position(pos), color{r, g, b} {}
    ~PointLight();
};

class SpotLight {
   private:
    PointLight point_light;
    Vector light_direction;  // direction of the light source
    double cutoff_angle;     // in degrees

   public:
    SpotLight(const Vector& pos, double r, double g, double b,
              const Vector& dir, double angle)
        : point_light(pos, r, g, b),
          light_direction(dir),
          cutoff_angle(angle) {}
    ~SpotLight();
};

#endif