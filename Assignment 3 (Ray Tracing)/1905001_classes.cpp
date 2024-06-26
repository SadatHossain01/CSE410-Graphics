#include "1905001_classes.h"

// Color

Color::Color(double r, double g, double b) : r(r), g(g), b(b) {}

void Color::clamp() {
    r = std::min(1.0, std::max(0.0, r));
    g = std::min(1.0, std::max(0.0, g));
    b = std::min(1.0, std::max(0.0, b));
}

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

const Vector& Vector::operator=(const Vector& v) {
    x = v.x, y = v.y, z = v.z;
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

Vector Object::get_reflection(const Vector& normal,
                              const Vector& incident) const {
    return incident - normal * 2 * incident.dot(normal);
}

int Object::get_next_reflection_object(Ray reflected_ray) const {
    // returns the index of the nearest object that the reflected ray intersects
    int nearest_idx = -1;
    double t_min_reflection = 1e9;

    for (int k = 0; k < objects.size(); k++) {
        Object* o = objects[k];
        double t = o->find_ray_intersection(reflected_ray);
        if (t > 0 && t < t_min_reflection) {
            t_min_reflection = t;
            nearest_idx = k;
        }
    }

    return nearest_idx;
}

Vector Object::get_refraction(const Vector& normal, const Vector& incident,
                              double n1, double n2) const {
    double n = n1 / n2;
    double cos_theta_i = -normal.dot(incident);
    double cos_theta_t = sqrt(1 - n * n * (1 - cos_theta_i * cos_theta_i));
    return incident * n + n * cos_theta_i - cos_theta_t * normal;
}

void Object::set_refractive_indices(double r, double g, double b) {
    red_refractive_index = r;
    green_refractive_index = g;
    blue_refractive_index = b;
}

void Object::shade(const Ray& ray, Color& color, int level) const {
    double t_intersect = find_ray_intersection(ray);
    if (level == 0 || t_intersect < 0) return;

    Vector intersection_point = ray.origin + ray.dir * t_intersect;
    Color object_local_color = get_color_at(intersection_point);

    // Ambient Component
    color = object_local_color * phong_coefficients.ambient;

    // Normal at intersection point
    Vector surface_normal = get_normal(intersection_point);
    if (ray.dir.dot(surface_normal) > 0) surface_normal = -surface_normal;

    // Both types of light sources
    for (LightSource* ls : light_sources) {
        Ray light_ray(ls->light_position,
                      intersection_point - ls->light_position);

        double beta;  // for spot light intensity
        if (ls->type == LightSource::SPOT) {
            // Continue with spot light unless the ray cast from light_position
            // to intersection_point exceeds the cutoff angle
            SpotLight* sls = (SpotLight*)ls;
            double dot = light_ray.dir.dot(sls->light_direction);
            double angle = acos(dot / (light_ray.dir.norm() *
                                       sls->light_direction.norm())) *
                           180.0 / PI;
            beta = fabs(angle * PI / 180);
            if (fabs(angle) >= sls->cutoff_angle) continue;
        }

        // Check if this ray is obscured by any other object
        // i.e. if this light ray reaches any other object before the current
        // one
        double t_cur = (intersection_point - ls->light_position).norm();
        if (t_cur < EPS) continue;  // light source is at the intersection point

        bool obscured = false;
        for (Object* obj : objects) {
            double t = obj->find_ray_intersection(light_ray);
            if (t > EPS && t + EPS < t_cur) {
                obscured = true;
                break;
            }
        }
        if (obscured) continue;

        // The light ray is not obscured by any other object

        // Diffuse Component
        // Calculate Lambert value using the surface normal and light ray
        double lambert_value =
            std::max(0.0, surface_normal.dot(-light_ray.dir));

        if (lambert_value < EPS) continue;

        double epsilon = 2;
        color += ls->color * phong_coefficients.diffuse * lambert_value *
                 object_local_color *
                 (ls->type == LightSource::SPOT ? pow(cos(beta), epsilon) : 1);

        // Specular Component
        // Find reflected ray for the light ray
        Ray reflected_ray(intersection_point,
                          get_reflection(surface_normal, light_ray.dir));
        // Calculate Phong value using the reflected ray and the view ray
        double phong_value = std::max(0.0, reflected_ray.dir.dot(-ray.dir));
        color += ls->color * phong_coefficients.specular *
                 pow(phong_value, phong_coefficients.shine) *
                 object_local_color *
                 (ls->type == LightSource::SPOT ? pow(cos(beta), epsilon) : 1);
    }

    if (level == 0) return;

    // Recursive Reflection
    Ray reflected_ray(intersection_point,
                      get_reflection(surface_normal, ray.dir));

    reflected_ray.origin +=
        reflected_ray.dir * EPS;  // To avoid self-reflection

    int next_reflection_object_idx = get_next_reflection_object(reflected_ray);
    if (next_reflection_object_idx == -1) return;

    Color reflected_color(0, 0, 0);
    objects[next_reflection_object_idx]->shade(reflected_ray, reflected_color,
                                               level - 1);
    color += reflected_color * phong_coefficients.reflection;
    return;
}

Object::~Object() {}



// Floor

Floor::Floor(double floor_width, double tile_width)
    : Object(Vector(-floor_width / 2.0, -floor_width / 2.0, 0.0)),
      floor_width(floor_width),
      tile_width(tile_width) {}

void Floor::draw() const {
    double cur_x = -floor_width / 2.0;
    double y_start = -floor_width / 2.0;
    int tile_count = floor_width / tile_width;

    glPushMatrix();
    {
        for (int i = 0; i < tile_count; i++) {
            double cur_y = y_start;
            for (int j = 0; j < tile_count; j++) {
                if ((i + j) % 2 == 0) glColor3f(0, 0, 0);
                else glColor3f(1, 1, 1);
                glBegin(GL_QUADS);
                {
                    glVertex3f(cur_x, cur_y, 0.0);
                    glVertex3f(cur_x + tile_width, cur_y, 0.0);
                    glVertex3f(cur_x + tile_width, cur_y + tile_width, 0.0);
                    glVertex3f(cur_x, cur_y + tile_width, 0.0);
                }
                glEnd();
                cur_y += tile_width;
            }
            cur_x += tile_width;
        }
    }
    glPopMatrix();
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

double Floor::find_ray_intersection(Ray ray) const {
    Vector normal = get_normal(reference_point);
    double denom = normal.dot(ray.dir);
    if (fabs(denom) < EPS) return -1.0;
    double t = -(normal.dot(ray.origin) - normal.dot(reference_point)) / denom;

    Vector intersection_point = ray.origin + ray.dir * t;
    if (intersection_point.x < reference_point.x ||
        intersection_point.x > reference_point.x + floor_width ||
        intersection_point.y < reference_point.y ||
        intersection_point.y > reference_point.y + floor_width)
        return -1.0;
    return t;
}

Vector Floor::get_normal(const Vector& point) const { return Vector(0, 0, 1); }


// Sphere

Sphere::Sphere(const Vector& center, double radius)
    : Object(center), radius(radius) {}

void Sphere::draw() const {
    glColor3f(color.r, color.g, color.b);
    glPushMatrix();
    glTranslatef(reference_point.x, reference_point.y, reference_point.z);
    glutSolidSphere(radius, 50, 50);
    glPopMatrix();
}

Vector Sphere::get_normal(const Vector& point) const {
    return (point - reference_point).normalize();
}

double Sphere::find_ray_intersection(Ray ray) const {
    Vector center_to_ray_origin = ray.origin - reference_point;

    // ray : the ray from eye/light source to the object
    double a = 1.0;
    double b = 2 * ray.dir.dot(center_to_ray_origin);
    double c = center_to_ray_origin.dot(center_to_ray_origin) - radius * radius;

    double discriminant = b * b - 4 * a * c;
    if (discriminant < 0) return -1.0;

    double t_minus = (-b - sqrt(discriminant)) / (2 * a);
    double t_plus = (-b + sqrt(discriminant)) / (2 * a);

    if (t_minus < 0 && t_plus < 0) return -1.0;
    if (t_minus < 0) return t_plus;
    if (t_plus < 0) return t_minus;
    return std::min(t_minus, t_plus);
}

void Sphere::print() const {
    std::cout << "Sphere at (" << reference_point.x << ", " << reference_point.y
              << ", " << reference_point.z << ") with radius " << radius
              << std::endl;
}

// Triangle

Triangle::Triangle(const Vector& a, const Vector& b, const Vector& c)
    : a(a), b(b), c(c) {}

void Triangle::draw() const {
    glColor3f(color.r, color.g, color.b);
    glBegin(GL_TRIANGLES);
    {
        glVertex3f(a.x, a.y, a.z);
        glVertex3f(b.x, b.y, b.z);
        glVertex3f(c.x, c.y, c.z);
    }
    glEnd();
}

double Triangle::find_ray_intersection(Ray ray) const {
    auto determinant = [](const double(&matrix)[3][3]) -> double {
        return matrix[0][0] *
                   (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1]) -
               matrix[0][1] *
                   (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0]) +
               matrix[0][2] *
                   (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]);
    };


    double beta_matrix[3][3] = {{a.x - ray.origin.x, a.x - c.x, ray.dir.x},
                                {a.y - ray.origin.y, a.y - c.y, ray.dir.y},
                                {a.z - ray.origin.z, a.z - c.z, ray.dir.z}};

    double gamma_matrix[3][3] = {{a.x - b.x, a.x - ray.origin.x, ray.dir.x},
                                 {a.y - b.y, a.y - ray.origin.y, ray.dir.y},
                                 {a.z - b.z, a.z - ray.origin.z, ray.dir.z}};

    double t_matrix[3][3] = {{a.x - b.x, a.x - c.x, a.x - ray.origin.x},
                             {a.y - b.y, a.y - c.y, a.y - ray.origin.y},
                             {a.z - b.z, a.z - c.z, a.z - ray.origin.z}};

    double A_matrix[3][3] = {{a.x - b.x, a.x - c.x, ray.dir.x},
                             {a.y - b.y, a.y - c.y, ray.dir.y},
                             {a.z - b.z, a.z - c.z, ray.dir.z}};

    double A_det = determinant(A_matrix);
    double beta = determinant(beta_matrix) / A_det;
    double gamma = determinant(gamma_matrix) / A_det;
    double t = determinant(t_matrix) / A_det;

    if (beta + gamma < 1 && beta > 0 && gamma > 0 && t > 0) return t;
    return -1.0;
}

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

void GeneralQuadraticSurface::draw() const {}

double GeneralQuadraticSurface::find_ray_intersection(Ray ray) const {
    /*
    Ax^2 + By^2 + Cz^2 + Dxy + Eyz + Fzx + Gx + Hy + Iz + J = 0
    Ray Equation:
    x = x0 + t * dx
    y = y0 + t * dy
    z = z0 + t * dz
    Where, (x0, y0, z0) is the origin of the ray and (dx, dy, dz) is the
    direction of the ray and t is the parameter.
    Substitute these values in the quadratic equation and solve for t
    */
    double a = A * ray.dir.x * ray.dir.x + B * ray.dir.y * ray.dir.y +
               C * ray.dir.z * ray.dir.z + D * ray.dir.x * ray.dir.y +
               E * ray.dir.y * ray.dir.z + F * ray.dir.z * ray.dir.x;
    double b = 2 * A * ray.origin.x * ray.dir.x +
               2 * B * ray.origin.y * ray.dir.y +
               2 * C * ray.origin.z * ray.dir.z +
               D * (ray.origin.x * ray.dir.y + ray.origin.y * ray.dir.x) +
               E * (ray.origin.y * ray.dir.z + ray.origin.z * ray.dir.y) +
               F * (ray.origin.z * ray.dir.x + ray.origin.x * ray.dir.z) +
               G * ray.dir.x + H * ray.dir.y + I * ray.dir.z;
    double c =
        A * ray.origin.x * ray.origin.x + B * ray.origin.y * ray.origin.y +
        C * ray.origin.z * ray.origin.z + D * ray.origin.x * ray.origin.y +
        E * ray.origin.y * ray.origin.z + F * ray.origin.z * ray.origin.x +
        G * ray.origin.x + H * ray.origin.y + I * ray.origin.z + J;

    auto valid = [&](double t) {
        Vector intersection_point = ray.origin + ray.dir * t;
        if (fabs(length) > EPS &&
            (intersection_point.x < reference_point.x - EPS ||
             intersection_point.x > reference_point.x + length + EPS))
            return false;
        if (fabs(width) > EPS &&
            (intersection_point.y < reference_point.y - EPS ||
             intersection_point.y > reference_point.y + width + EPS))
            return false;
        if (fabs(height) > EPS &&
            (intersection_point.z < reference_point.z - EPS ||
             intersection_point.z > reference_point.z + height + EPS))
            return false;
        return true;
    };

    double discriminant = b * b - 4 * a * c;
    if (discriminant < 0) return -1.0;
    double t_minus = (-b - sqrt(discriminant)) / (2 * a);
    double t_plus = (-b + sqrt(discriminant)) / (2 * a);

    if (t_minus < 0) {
        if (t_plus < 0) return -1.0;
        return valid(t_plus) ? t_plus : -1.0;
    } else {
        if (valid(t_minus)) return t_minus;
        if (t_plus < 0) return -1.0;
        return valid(t_plus) ? t_plus : -1.0;
    }
}

Vector GeneralQuadraticSurface::get_normal(const Vector& point) const {
    return Vector(2 * A * point.x + D * point.y + F * point.z + G,
                  2 * B * point.y + D * point.x + E * point.z + H,
                  2 * C * point.z + E * point.y + F * point.x + I)
        .normalize();
}

void GeneralQuadraticSurface::print() const {
    std::cout << "General Quadratic Surface at (" << reference_point.x << ", "
              << reference_point.y << ", " << reference_point.z
              << ") with length " << length << ", width " << width
              << ", height " << height << std::endl;
}

// Prism

Prism::Prism(const Vector& a, const Vector& b, const Vector& c, const Vector& d,
             const Vector& e, const Vector& f)
    : a(a), b(b), c(c), d(d), e(e), f(f) {}

void Prism::draw() const {
    const double EPS = 0.01;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(color.r, color.g, color.b, 0.8);

    glBegin(GL_TRIANGLES);
    {
        glVertex3f(a.x, a.y, a.z);
        glVertex3f(b.x, b.y, b.z);
        glVertex3f(c.x, c.y, c.z);
    }
    glEnd();

    glBegin(GL_TRIANGLES);
    {
        glVertex3f(d.x, d.y, d.z);
        glVertex3f(e.x, e.y, e.z);
        glVertex3f(f.x, f.y, f.z);
    }
    glEnd();

    glBegin(GL_QUADS);
    {
        // bottom one
        glVertex3f(a.x, a.y, a.z + EPS);
        glVertex3f(b.x, b.y, b.z + EPS);
        glVertex3f(e.x, e.y, e.z + EPS);
        glVertex3f(d.x, d.y, d.z + EPS);

        // left one
        glVertex3f(b.x + EPS, b.y, b.z);
        glVertex3f(c.x + EPS, c.y, c.z);
        glVertex3f(f.x + EPS, f.y, f.z);
        glVertex3f(e.x + EPS, e.y, e.z);

        // right one
        glVertex3f(c.x - EPS, c.y, c.z);
        glVertex3f(a.x - EPS, a.y, a.z);
        glVertex3f(d.x - EPS, d.y, d.z);
        glVertex3f(f.x - EPS, f.y, f.z);
    }
    glEnd();

    glDisable(GL_BLEND);
}

Vector Prism::get_normal(const Vector& point) const {
    auto is_on_triangular_plane = [](const Vector& a, const Vector& b,
                                     const Vector& c, const Vector& p) -> bool {
        Vector u = b - a;
        Vector v = c - a;
        Vector w = p - a;
        Vector normal = u.cross(v);
        double dot = normal.dot(normal);
        double gamma = (u.cross(w)).dot(normal) / dot;
        double beta = (w.cross(v)).dot(normal) / dot;
        double alpha = 1 - beta - gamma;
        return alpha >= -EPS && beta >= -EPS && gamma >= -EPS;
    };

    auto is_on_rectangular_plane = [&](const Vector& a, const Vector& b,
                                       const Vector& c, const Vector& d,
                                       const Vector& p) -> bool {
        return is_on_triangular_plane(a, b, c, p) ||
               is_on_triangular_plane(a, c, d, p);
    };

    auto triangular_normal = [](const Vector& a, const Vector& b,
                                const Vector& c) -> Vector {
        return (b - a).cross(c - a).normalize();
    };

    auto rectangular_normal = [&](const Vector& a, const Vector& b,
                                  const Vector& c, const Vector& d) -> Vector {
        if (is_on_triangular_plane(a, b, c, point))
            return triangular_normal(a, b, c);
        return triangular_normal(a, c, d);
    };

    if (is_on_triangular_plane(a, b, c, point))
        return triangular_normal(a, b, c);
    if (is_on_triangular_plane(d, e, f, point))
        return triangular_normal(d, e, f);
    if (is_on_rectangular_plane(a, b, e, d, point))
        return rectangular_normal(a, b, e, d);
    if (is_on_rectangular_plane(b, c, f, e, point))
        return rectangular_normal(b, c, f, e);
    if (is_on_rectangular_plane(c, a, d, f, point))
        return rectangular_normal(c, a, d, f);
    throw std::invalid_argument("Point is not on the prism");
}

void Prism::shade(const Ray& ray, Color& color, int level) const {
    double t_intersect = find_ray_intersection(ray);
    if (level == 0 || t_intersect < 0) return;

    Vector intersection_point = ray.origin + ray.dir * t_intersect;
    Color local_color = get_color_at(intersection_point);

    // Ambient Component
    color = local_color * phong_coefficients.ambient;

    // Normal at intersection point
    Vector surface_normal = get_normal(intersection_point);
    if (ray.dir.dot(surface_normal) > 0)
        surface_normal = -surface_normal;  // mainly for triangle, floor and
                                           // general quadratic surface

    for (LightSource* ls : light_sources) {
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
                           180.0 / PI;
            if (fabs(angle) >= sls->cutoff_angle) continue;
        }

        // Check if this ray is obscured by any other object
        // that is, if this ray reaches any other object before the current one
        double t_cur = (intersection_point - ls->light_position).norm();
        if (t_cur < EPS)
            continue;  // light source is at the intersection point or in front

        bool obscured = false;
        for (Object* obj : objects) {
            double t = obj->find_ray_intersection(light_ray);
            if (t > EPS && t + EPS < t_cur) {
                obscured = true;
                break;
            }
        }
        if (obscured) continue;

        // So, the light ray is not obscured by any other object

        // Diffuse Component
        // Calculate Lambert value using the surface normal and light ray
        double lambert_value =
            std::max(0.0, surface_normal.dot(-light_ray.dir));
        color += ls->color * phong_coefficients.diffuse * lambert_value *
                 local_color;

        // Specular Component
        // Find reflected ray for the light ray
        Ray reflected_ray(intersection_point,
                          get_reflection(surface_normal, light_ray.dir));
        // Calculate Phong value using the reflected ray and the view ray
        double phong_value = std::max(0.0, reflected_ray.dir.dot(-ray.dir));
        color += ls->color * phong_coefficients.specular *
                 pow(phong_value, phong_coefficients.shine) * local_color;
    }

    if (level == 0) return;

    // Recursive Reflection
    Ray reflected_ray(
        intersection_point,
        get_reflection(surface_normal, ray.dir));  // Reflected Ray

    reflected_ray.origin +=
        reflected_ray.dir * EPS;                   // To avoid self-reflection

    int next_reflection_object_idx = get_next_reflection_object(reflected_ray);
    if (next_reflection_object_idx == -1) return;

    Color reflected_color(0, 0, 0);
    objects[next_reflection_object_idx]->shade(reflected_ray, reflected_color,
                                               level - 1);
    color += reflected_color * phong_coefficients.reflection;
    return;
}

double Prism::find_ray_intersection(Ray ray) const {
    double t_min = 1e9;
    std::vector<Triangle> triangles = {Triangle(a, b, c), Triangle(d, e, f),
                                       Triangle(a, b, d), Triangle(b, d, e),
                                       Triangle(a, c, d), Triangle(c, d, f),
                                       Triangle(b, c, e), Triangle(c, e, f)};
    for (const Triangle& triangle : triangles) {
        double t = triangle.find_ray_intersection(ray);
        if (t > -EPS && t < t_min) t_min = t;
    }
    return t_min > 9e8 ? -1.0 : t_min;
}

void Prism::print() const {
    std::cout << "Prism with vertices " << a << ", " << b << ", " << c << ", "
              << d << ", " << e << ", " << f << std::endl;
}

// Light Source

LightSource::LightSource(const Vector& pos, double r, double g, double b,
                         LightType type)
    : light_position(pos), color(r, g, b), type(type) {}
LightSource::~LightSource() {}


// Point Light

PointLight::PointLight(const Vector& pos, double r, double g, double b)
    : LightSource(pos, r, g, b, POINT) {}

void PointLight::draw() const {
    glColor3f(1, 1, 0);
    glPushMatrix();
    glTranslatef(light_position.x, light_position.y, light_position.z);
    glutSolidSphere(4, 50, 50);
    glPopMatrix();
}



// Spot Light

SpotLight::SpotLight(const Vector& pos, double r, double g, double b,
                     const Vector& dir, double angle)
    : LightSource(pos, r, g, b, SPOT), cutoff_angle(angle) {
    light_direction = dir.normalize();
}

void SpotLight::draw() const {
    glColor3f(0, 1, 1);
    glPushMatrix();
    glTranslatef(light_position.x, light_position.y, light_position.z);
    glutSolidSphere(2, 50, 50);
    glPopMatrix();
}
