#pragma once
#include "dependancies/glm/glm.hpp"

#include <memory>

#include "ray.hpp"

class Material; 

class hit_details {
public:
    glm::vec3 p;
    glm::vec3 normal; 
    std::shared_ptr<Material> mat_ptr; 
    double t;
    double u;
    double v;
    bool front_face; 

    glm::vec3 first_albedo; 
    glm::vec3 first_normal; 

    void set_face(const Ray& ray, const glm::vec3& outward_normal); 
};

double random_double();

double random_double(double min, double max);

int random_int(int min, int max);

double clamp(double x, double min, double max);

glm::vec3 random_in_unit_sphere();

glm::vec3 random_unit_vector();

glm::vec3 random_in_unit_disk();

glm::vec3 random_in_hemisphere(const glm::vec3& normal);

bool near_zero(glm::vec3& e);