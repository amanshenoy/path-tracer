#include "background.hpp"

glm::vec3 GradientBG::get_color(const Ray& ray_in){
    glm::vec3 unit_direction = abs(glm::normalize(ray_in.direction()));
    float t = gradient * unit_direction.y;
    return (1 - t) * low + t * high;
}