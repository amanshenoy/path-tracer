#include <iostream>

#include "dependancies/glm/glm.hpp"

#include "utils.hpp"

void hit_details::set_face(const Ray& ray, const glm::vec3& outward_normal) {
    front_face = glm::dot(ray.direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
}

double random_double() {
    return rand() / (RAND_MAX + 1.0);
}

double random_double(double min, double max) {
    return min + (max-min)*random_double();
}

double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

glm::vec3 random_in_unit_sphere() {
    while (true) {
        glm::vec3 p(random_double(-1.0, 1.0), random_double(-1.0, 1.0), random_double(-1.0, 1.0));
        if (glm::length(p) >= 1) continue;
        return p;
    }
}

glm::vec3 random_unit_vector() {
    return glm::normalize(random_in_unit_sphere());
}

glm::vec3 random_in_unit_disk() {
    while (true) {
        auto p = glm::vec3(random_double(-1,1), random_double(-1,1), 0);
        if (pow(glm::length(p), 2) >= 1) continue;
        return p;
    }
}

glm::vec3 random_in_hemisphere(const glm::vec3& normal) {
    glm::vec3 in_unit_sphere = random_in_unit_sphere();
    if (glm::dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}

int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return static_cast<int>(random_double(min, max+1));
}

bool near_zero(glm::vec3& e) {
    // Return true if the vector is close to zero in all dimensions.
    const auto s = 1e-8;
    return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
}