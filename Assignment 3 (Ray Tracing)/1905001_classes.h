#ifndef CLASSES_H
#define CLASSES_H

#include <GL/glut.h>
#include <bits/stdc++.h>

// Forward Declarations
struct Color;
struct PhongCoefficients;
struct Vector;
struct Ray;
struct Camera;
class Object;
class Sphere;
class Triangle;
class GeneralQuadraticSurface;
class Prism;
class Floor;
struct LightSource;
struct PointLight;
struct SpotLight;

const double PI = 2 * acos(0.0);
const double EPS = 1e-6;

extern std::vector<Object*> objects;
extern std::vector<LightSource*> light_sources;

struct Color {
   public:
    double r, g, b;
    Color(double r = 0, double g = 0, double b = 0);
    Color operator+(const Color& c) const;
    Color operator+=(const Color& c);
    Color operator*(const double& d) const;
    Color operator*(const Color& c) const;
    Color operator*=(const double& d);
    void clamp();
    friend Color operator*(const double& d, const Color& c);
};

struct PhongCoefficients {
   public:
    double ambient, diffuse, specular, reflection;
    int shine;
    PhongCoefficients(double ambient = 0, double diffuse = 0,
                      double specular = 0, double reflection = 0,
                      int shine = 0);
};

struct Vector {
   public:
    double x, y, z;
    Vector(double x = 0, double y = 0, double z = 0);
    Vector operator+(const Vector& v) const;
    Vector operator+=(const Vector& v);
    Vector operator-(const Vector& v) const;
    Vector operator-=(const Vector& v);
    Vector operator-() const;
    Vector operator*(const double& d) const;
    friend Vector operator*(const double& d, const Vector& v);
    Vector operator*=(const double& d);
    Vector operator/(const double& d) const;
    Vector operator/=(const double& d);
    const Vector& operator=(const Vector& v);
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
           const Vector& look_at = Vector(0, 0, 0),
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

struct Ray {
   public:
    Vector origin, dir;
    Ray(const Vector& start, const Vector& dir);
};

class Object {
   protected:
    Vector reference_point;
    Color color;
    PhongCoefficients phong_coefficients;
    double red_refractive_index, green_refractive_index, blue_refractive_index;
    Vector get_reflection(const Vector& normal, const Vector& incident) const;
    int get_next_reflection_object(Ray reflected_ray) const;
    Vector get_refraction(const Vector& normal, const Vector& incident,
                          double n1, double n2) const;

   public:
    Object(const Vector& ref = Vector(0, 0, 0));
    virtual void draw() const = 0;
    virtual Vector get_normal(const Vector& point) const = 0;
    virtual Color get_color_at(const Vector& point) const;
    virtual void shade(const Ray& ray, Color& color, int level) const;
    virtual double find_ray_intersection(Ray ray) const = 0;
    void set_color(double r, double g, double b);
    void set_shine(int shine);
    void set_coefficients(double ambient, double diffuse, double specular,
                          double reflection);
    void set_refractive_indices(double r, double g, double b);
    virtual void print() const = 0;
    virtual ~Object();
};

class Floor : public Object {
   public:
    double floor_width, tile_width;
    Floor(double floor_width, double tile_width);
    void draw() const override;
    Vector get_normal(const Vector& point) const override;
    Color get_color_at(const Vector& pt) const override;
    double find_ray_intersection(Ray ray) const override;
    void print() const override;
};

class Sphere : public Object {
   public:
    double radius;
    Sphere(const Vector& center, double radius);
    void draw() const override;
    Vector get_normal(const Vector& point) const override;
    double find_ray_intersection(Ray ray) const override;
    void print() const override;
};

class Triangle : public Object {
   public:
    Vector a, b, c;
    Triangle(const Vector& a, const Vector& b, const Vector& c);
    void draw() const override;
    Vector get_normal(const Vector& point) const override;
    double find_ray_intersection(Ray ray) const override;
    void print() const override;
};

class GeneralQuadraticSurface : public Object {
    // Ax^2 + By^2 + Cz^2 + Dxy + Eyz + Fzx + Gx + Hy + Iz + J = 0
   public:
    double A, B, C, D, E, F, G, H, I, J;
    double length, width, height;
    GeneralQuadraticSurface(double A, double B, double C, double D, double E,
                            double F, double G, double H, double I, double J,
                            const Vector& ref, double l, double w, double h);
    void draw() const override;
    Vector get_normal(const Vector& point) const override;
    double find_ray_intersection(Ray ray) const override;
    void print() const override;
};

class Prism : public Object {
   public:
    Vector a, b, c, d, e, f;
    Prism(const Vector& a, const Vector& b, const Vector& c, const Vector& d,
          const Vector& e, const Vector& f);
    void draw() const override;
    void shade(const Ray& ray, Color& color, int level) const override;
    Vector get_normal(const Vector& point) const override;
    double find_ray_intersection(Ray ray) const override;
    void print() const override;
};

struct LightSource {
   public:
    Color color;
    Vector light_position;
    enum LightType { POINT, SPOT } type;
    LightSource(const Vector& pos, double r, double g, double b,
                LightType type);
    virtual void draw() const = 0;
    virtual ~LightSource();
};

struct PointLight : public LightSource {
   public:
    PointLight(const Vector& pos, double r, double g, double b);
    void draw() const override;
};

struct SpotLight : public LightSource {
   public:
    Vector light_direction;
    double cutoff_angle;  // in degrees
    SpotLight(const Vector& pos, double r, double g, double b,
              const Vector& dir, double angle);
    void draw() const override;
};

#endif