#include "1905001_classes.h"

const double PI = 2 * acos(0.0);
const double EPS = 1e-8;

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

// Object
Object::~Object() {}

// Floor
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
                glBegin(GL_QUADS);
                {
                    glVertex3f(0, 0, 0);
                    glVertex3f(tile_width, 0, 0);
                    glVertex3f(tile_width, tile_width, 0);
                    glVertex3f(0, tile_width, 0);
                }
                glEnd();
            }
            glPopMatrix();
        }
    }
}

// Sphere
void Sphere::draw() {}

// Triangle
void Triangle::draw() {}

// GeneralQuadraticSurface
void GeneralQuadraticSurface::draw() {}

PointLight::~PointLight() {}
SpotLight::~SpotLight() {}