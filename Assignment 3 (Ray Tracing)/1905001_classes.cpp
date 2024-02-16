#include "1905001_classes.h"

// Color

Color::Color(double r, double g, double b) : r(r), g(g), b(b) {}

Color Color::operator+(const Color& c) const {
    return Color(r + c.r, g + c.g, b + c.b);
}

Color Color::operator+=(const Color& c) {
    r += c.r, g += c.g, b += c.b;
    return *this;
}

Color Color::operator*(const double& d) const {
    return Color(r * d, g * d, b * d);
}

Color Color::operator*(const Color& c) const {
    Color ret(r * c.r, g * c.g, b * c.b);
    assert(ret.r >= -EPS && ret.r <= 1 + EPS && ret.g >= -EPS &&
           ret.g <= 1 + EPS && ret.b >= -EPS && ret.b <= 1 + EPS);
    return ret;
}

Color operator*(const double& d, const Color& c) {
    return Color(c.r * d, c.g * d, c.b * d);
}

Color Color::operator*=(const double& d) {
    r *= d, g *= d, b *= d;
    return *this;
}



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

Vector Vector::operator-() const { return Vector(-x, -y, -z); }

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

Ray::Ray(const Vector& start, const Vector& dir) : origin(start) {
    this->dir = dir.normalize();
}


// Object

Object::Object(const Vector& ref) : reference_point(ref) {}

Color Object::get_color_at(const Vector& pt) const { return color; }

void Object::set_color(double r, double g, double b) { color = Color(r, g, b); }

void Object::set_shine(int shine) { phong_coefficients.shine = shine; }

void Object::set_coefficients(double ambient, double diffuse, double specular,
                              double reflection) {
    phong_coefficients = PhongCoefficients(
        ambient, diffuse, specular, reflection, phong_coefficients.shine);
}

double Object::intersect(const Ray& ray, Color& color, int level) {
    double t_min = find_ray_intersection(ray);
    if (level == 0)
        return t_min;  // determine the nearest object only, no color

    Vector intersection_point = ray.origin + ray.dir * t_min;
    Color local_color = get_color_at(intersection_point);

    // Ambient Component
    color = local_color * phong_coefficients.ambient;

    Ray surface_normal(intersection_point, get_normal(intersection_point));

    for (auto ls : light_sources) {
        Ray light_ray(
            ls->light_position,
            intersection_point -
                ls->light_position);  // In Lambert's cosine law, the light ray
                                      // is from the intersection point to the
                                      // light source

        if (ls->type == LightSource::SPOT) {
            // Continue with spot light unless the ray cast from light_position
            // to intersection_point exceeds the cutoff angle
            SpotLight* sls = (SpotLight*)ls;
            double dot = light_ray.dir.dot(sls->light_direction);
            double angle = acos(dot / (light_ray.dir.norm() *
                                       sls->light_direction.norm())) *
                           180 / PI;
            if (angle > sls->cutoff_angle) continue;
        }

        // Check if this ray is obscured by any other object
        double t_cur = (intersection_point - ls->light_position).norm();
        if (t_cur < EPS)
            continue;  // light source is literally at the intersection point

        bool obscured = false;
        for (auto obj : objects) {
            double t = obj->find_ray_intersection(light_ray);
            if (t > EPS && t + EPS < t_cur) {
                obscured = true;
                break;
            }
        }
        if (obscured) continue;

        // So, the light ray is not obscured by any other object

        // Calculate Lambert value using the surface normal and light ray
        double lambert = std::max(0.0, surface_normal.dir.dot(-light_ray.dir));

        // Find reflected ray for the light ray
        Ray reflected_ray(
            intersection_point,
            light_ray.dir -
                2 * surface_normal.dir.dot(light_ray.dir) *
                    surface_normal
                        .dir);  // Again, because the light ray should be from
                                // the intersection point to the light source

        // Calculate Phong value using the reflected ray and the view ray
        double phong = std::max(0.0, reflected_ray.dir.dot(-ray.dir));

        // Diffuse Component
        color += ls->color * phong_coefficients.diffuse * lambert * local_color;

        // Specular Component
        color += ls->color * phong_coefficients.specular *
                 pow(phong, phong_coefficients.shine) * local_color;
    }

    if (level >= reflection_depth) return t_min;
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

void Floor::print() const {
    std::cout << "Floor at (" << reference_point.x << ", " << reference_point.y
              << ", " << reference_point.z << ") with floor width "
              << floor_width << " and tile width " << tile_width << std::endl;
}

Color Floor::get_color_at(const Vector& pt) const {
    int i = (pt.x - reference_point.x) / tile_width;
    int j = (pt.y - reference_point.y) / tile_width;
    if ((i + j) % 2 == 0) return Color(0, 0, 0);
    return Color(1, 1, 1);
}

double Floor::find_ray_intersection(const Ray& ray) { return -1.0; }

Vector Floor::get_normal(const Vector& point) const { return Vector(0, 0, 1); }

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

Vector Sphere::get_normal(const Vector& point) const {
    return (point - reference_point).normalize();
}

double Sphere::find_ray_intersection(const Ray& ray) {
    /*
    https://kylehalladay.com/blog/tutorial/math/2013/12/24/Ray-Sphere-Intersection.html
    t1, t2: the distance from the ray origin to the intersection points (P1,
    P2) tc: the distance from the ray origin to the point (P') on the ray
    halfway between the 2 intersection points t1c: distance from the closest
    intersection point (P1) to the point (P')
    */
    Vector origin_center = reference_point - ray.origin;
    double tc = origin_center.dot(ray.dir);
    if (tc < EPS) return -1;
    // d: the perpendicular distance from the center to P'
    double d = sqrt(tc * tc - origin_center.dot(origin_center));
    if (d > radius - EPS) return -1;
    double t1c = sqrt(radius * radius - d * d);
    double t1 = tc - t1c;
    double t2 = tc + t1c;
    if (t1 > t2) std::swap(t1, t2);
    if (t2 < 0) return -1;
    return t1 > 0 ? t1 : t2;
}

void Sphere::print() const {
    std::cout << "Sphere at (" << reference_point.x << ", " << reference_point.y
              << ", " << reference_point.z << ") with radius " << radius
              << std::endl;
}

// Triangle

Triangle::Triangle(const Vector& a, const Vector& b, const Vector& c)
    : a(a), b(b), c(c) {}

void Triangle::draw() {
    glColor3f(color.r, color.g, color.b);
    draw_triangle(a, b, c);
}

double Triangle::find_ray_intersection(const Ray& ray) { return -1.0; }

Vector Triangle::get_normal(const Vector& point) const {
    return (b - a).cross(c - a).normalize();
}

void Triangle::print() const {
    std::cout << "Triangle with vertices " << a << ", " << b << ", " << c
              << std::endl;
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

double GeneralQuadraticSurface::find_ray_intersection(const Ray& ray) {
    return -1.0;
}

Vector GeneralQuadraticSurface::get_normal(const Vector& point) const {
    return Vector(0, 0, 0);
}

void GeneralQuadraticSurface::print() const {
    std::cout << "General Quadratic Surface at (" << reference_point.x << ", "
              << reference_point.y << ", " << reference_point.z
              << ") with length " << length << ", width " << width
              << ", height " << height << std::endl;
}

// Light Source

LightSource::LightSource(const Vector& pos, double r, double g, double b,
                         LightType type)
    : light_position(pos), color(r, g, b), type(type) {}


// Point Light

PointLight::PointLight(const Vector& pos, double r, double g, double b)
    : LightSource(pos, r, g, b, POINT) {}

PointLight::~PointLight() {}



// Spot Light

SpotLight::SpotLight(const Vector& pos, double r, double g, double b,
                     const Vector& dir, double angle)
    : LightSource(pos, r, g, b, SPOT), cutoff_angle(angle) {
    light_direction = dir.normalize();
}

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