#include "1905001_classes.h"

// Color
Color::Color(double r, double g, double b) : r(r), g(g), b(b) {}

// Phong Coefficients
PhongCoefficients::PhongCoefficients(double ambient, double diffuse,
                                     double specular, double reflection,
                                     int shine)
    : ambient(ambient),
      diffuse(diffuse),
      specular(specular),
      reflection(reflection),
      shine(shine) {}

// Vector
Vector::Vector(double x, double y, double z) : x(x), y(y), z(z) {}

Vector Vector::operator+(const Vector& v) const {
    return Vector(x + v.x, y + v.y, z + v.z);
}

Vector Vector::operator+=(const Vector& v) {
    x += v.x, y += v.y, z += v.z;
    return *this;
}

Vector Vector::operator-(const Vector& v) const {
    return Vector(x - v.x, y - v.y, z - v.z);
}

Vector Vector::operator-=(const Vector& v) {
    x -= v.x, y -= v.y, z -= v.z;
    return *this;
}

Vector Vector::operator*(const double& d) const {
    return Vector(x * d, y * d, z * d);
}

Vector operator*(const double& d, const Vector& v) {
    return Vector(v.x * d, v.y * d, v.z * d);
}

Vector Vector::operator*=(const double& d) {
    x *= d, y *= d, z *= d;
    return *this;
}

Vector Vector::operator/(const double& d) const {
    if (fabs(d) <= EPS) throw std::invalid_argument("Division by zero");
    return Vector(x / d, y / d, z / d);
}

Vector Vector::operator/=(const double& d) {
    if (fabs(d) <= EPS) throw std::invalid_argument("Division by zero");
    x /= d, y /= d, z /= d;
    return *this;
}

double Vector::dot(const Vector& v) const {
    return x * v.x + y * v.y + z * v.z;
}

Vector Vector::cross(const Vector& v) const {
    return Vector(y * v.z - v.y * z, v.x * z - x * v.z, x * v.y - v.x * y);
}

Vector Vector::normalize() const {
    double length = sqrt(x * x + y * y + z * z);
    if (fabs(length) <= EPS)
        throw std::invalid_argument("Vector magnitude is 0");
    return Vector(x / length, y / length, z / length);
}

Vector Vector::rotate(const Vector& axis, double angle) const {
    // Rodrigues' Rotation Formula
    double theta = angle * PI / 180;
    Vector k = axis.normalize();
    Vector v1 = *this * cos(theta);
    Vector v2 = k.cross(*this) * sin(theta);
    Vector v3 = k * k.dot(*this) * (1 - cos(theta));
    return v1 + v2 + v3;
}

bool Vector::check_normalized() const {
    double length = sqrt(x * x + y * y + z * z);
    return fabs(length - 1) <= EPS;
}

bool Vector::check_orthogonal(const Vector& v) const {
    return fabs(this->dot(v)) <= EPS;
}

double Vector::norm() const { return sqrt(x * x + y * y + z * z); }

double distance(const Vector& a, const Vector& b) { return (a - b).norm(); }

std::istream& operator>>(std::istream& is, Vector& v) {
    is >> v.x >> v.y >> v.z;
    return is;
}

std::ostream& operator<<(std::ostream& os, const Vector& v) {
    os << "Vector(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

// Camera
Camera::Camera(const Vector& eye, const Vector& look_at,
               const Vector& up_vector, double speed, double rotation_speed) {
    this->speed = speed;
    this->rotation_speed = rotation_speed;
    pos = eye;
    look = Vector(look_at.x - eye.x, look_at.y - eye.y, look_at.z - eye.z)
               .normalize();
    if (fabs(look.dot(up_vector)) <= EPS) {
        up = up_vector.normalize();
        right = look.cross(up).normalize();
    } else {
        right.x = look.y, right.y = -look.x, right.z = 0;
        right = right.normalize();
        up = right.cross(look).normalize();
    }
}

void Camera::move_forward() { pos += speed * look; }
void Camera::move_backward() { pos -= speed * look; }
void Camera::move_left() { pos -= speed * right; }
void Camera::move_right() { pos += speed * right; }
void Camera::move_up() { pos += speed * up; }
void Camera::move_down() { pos -= speed * up; }
void Camera::look_left() {
    look = look.rotate(up, rotation_speed);
    right = right.rotate(up, rotation_speed);
}
void Camera::look_right() {
    look = look.rotate(up, -rotation_speed);
    right = right.rotate(up, -rotation_speed);
}
void Camera::look_up() {
    look = look.rotate(right, rotation_speed);
    up = up.rotate(right, rotation_speed);
}
void Camera::look_down() {
    look = look.rotate(right, -rotation_speed);
    up = up.rotate(right, -rotation_speed);
}
void Camera::tilt_clockwise() {
    right = right.rotate(look, rotation_speed);
    up = up.rotate(look, rotation_speed);
}
void Camera::tilt_counterclockwise() {
    right = right.rotate(look, -rotation_speed);
    up = up.rotate(look, -rotation_speed);
}
void Camera::move_up_same_ref() {
    double prev_dist = distance(pos, Vector(0, 0, 0));
    pos.z += speed;
    double cur_dist = distance(pos, Vector(0, 0, 0));
    // cosine law to find the angle between previous and current look vector
    double angle =
        acos((prev_dist * prev_dist + cur_dist * cur_dist - speed * speed) /
             (2 * prev_dist * cur_dist));
    angle = 180 * angle / PI;
    look = look.rotate(right, -angle);
    up = up.rotate(right, -angle);
    right = look.cross(up).normalize();
}
void Camera::move_down_same_ref() {
    double prev_dist = distance(pos, Vector(0, 0, 0));
    pos.z -= speed;
    double cur_dist = distance(pos, Vector(0, 0, 0));
    // cosine law to find the angle between previous and current look vector
    double angle =
        acos((prev_dist * prev_dist + cur_dist * cur_dist - speed * speed) /
             (2 * prev_dist * cur_dist));
    angle = 180 * angle / PI;
    look = look.rotate(right, angle);
    up = up.rotate(right, angle);
    right = look.cross(up).normalize();
}

// Ray
Ray::Ray(const Vector& start, const Vector& dir) : start(start), dir(dir) {}

// Object
Object::Object(const Vector& ref) : reference_point(ref) {}
void Object::set_color(double r, double g, double b) { color = Color(r, g, b); }
void Object::set_shine(int shine) { phong_coefficients.shine = shine; }
void Object::set_coefficients(double ambient, double diffuse, double specular,
                              double reflection) {
    phong_coefficients = PhongCoefficients(
        ambient, diffuse, specular, reflection, phong_coefficients.shine);
}
double Object::intersect(const Ray& ray, const Color& color, int level) {
    return -1.0;
}
Object::~Object() {}

// Floor
Floor::Floor(double floor_width, double tile_width)
    : Object(Vector(-floor_width / 2.0, -floor_width / 2.0, 0.0)),
      floor_width(floor_width),
      tile_width(tile_width) {}
void Floor::draw() {
    int tile_count = floor_width / tile_width;
    for (int i = 0; i < tile_count; i++) {
        for (int j = 0; j < tile_count; j++) {
            glPushMatrix();
            {
                if ((i + j) % 2 == 0) glColor3f(0, 0, 0);
                else glColor3f(1, 1, 1);
                glTranslatef(this->reference_point.x + i * tile_width,
                             this->reference_point.y + j * tile_width, 0);
                draw_quad(Vector(0, 0, 0), Vector(tile_width, 0, 0),
                          Vector(tile_width, tile_width, 0),
                          Vector(0, tile_width, 0));
            }
            glPopMatrix();
        }
    }
}

// Sphere
Sphere::Sphere(const Vector& center, double radius)
    : Object(center), radius(radius) {}
void Sphere::draw() {
    glColor3f(color.r, color.g, color.b);
    glPushMatrix();
    glTranslatef(reference_point.x, reference_point.y, reference_point.z);
    draw_sphere(radius, 25, 25);
    glPopMatrix();
}

// Triangle
Triangle::Triangle(const Vector& a, const Vector& b, const Vector& c)
    : a(a), b(b), c(c) {}
void Triangle::draw() {
    glColor3f(color.r, color.g, color.b);
    draw_triangle(a, b, c);
}

// GeneralQuadraticSurface
GeneralQuadraticSurface::GeneralQuadraticSurface(double A, double B, double C,
                                                 double D, double E, double F,
                                                 double G, double H, double I,
                                                 double J, const Vector& ref,
                                                 double l, double w, double h)
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
void GeneralQuadraticSurface::draw() {}

// Point Light
PointLight::PointLight(const Vector& pos, double r, double g, double b)
    : light_position(pos), color{r, g, b} {}
PointLight::~PointLight() {}

// Spot Light
SpotLight::SpotLight(const Vector& pos, double r, double g, double b,
                     const Vector& dir, double angle)
    : point_light(pos, r, g, b), light_direction(dir), cutoff_angle(angle) {}
SpotLight::~SpotLight() {}

// Helper Functions
void draw_line(const Vector& a, const Vector& b) {
    glBegin(GL_LINES);
    {
        glVertex3f(a.x, a.y, a.z);
        glVertex3f(b.x, b.y, b.z);
    }
    glEnd();
}

void draw_triangle(const Vector& a, const Vector& b, const Vector& c) {
    glBegin(GL_TRIANGLES);
    {
        glVertex3f(a.x, a.y, a.z);
        glVertex3f(b.x, b.y, b.z);
        glVertex3f(c.x, c.y, c.z);
    }
    glEnd();
}

void draw_quad(const Vector& a, const Vector& b, const Vector& c,
               const Vector& d) {
    glBegin(GL_QUADS);
    {
        glVertex3f(a.x, a.y, a.z);
        glVertex3f(b.x, b.y, b.z);
        glVertex3f(c.x, c.y, c.z);
        glVertex3f(d.x, d.y, d.z);
    }
    glEnd();
}

void draw_sphere(double radius, int stack_count, int sector_count) {
    double stack_step = PI / stack_count;
    double sector_step = 2 * PI / sector_count;

    std::vector<Vector> points;

    for (int i = 0; i <= stack_count; i++) {
        double stack_angle = PI / 2.0 - i * stack_step;  // [-π/2, π/2]
        for (int j = 0; j <= sector_count; j++) {
            double sector_angle = j * sector_step;       // [0, 2π]

            double x = radius * cos(stack_angle) * cos(sector_angle);
            double y = radius * cos(stack_angle) * sin(sector_angle);
            double z = radius * sin(stack_angle);

            points.push_back(Vector(x, y, z));
        }
    }

    for (int i = 0; i < stack_count; i++) {
        int k1 = i * (sector_count + 1);
        int k2 = k1 + sector_count + 1;
        for (int j = 0; j < sector_count; j++) {
            if (i != 0)
                draw_triangle(points[k1 + j], points[k2 + j],
                              points[k1 + 1 + j]);
            if (i != stack_count - 1)
                draw_triangle(points[k1 + 1 + j], points[k2 + j],
                              points[k2 + 1 + j]);

            draw_line(points[j + k1], points[j + k2]);
            if (i != 0) draw_line(points[j + k1], points[j + k1 + 1]);
        }
    }

    points.clear();
}