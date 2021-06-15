#include "ray.hpp"

namespace core {
    glm::vec3 Ray::origin() const {
        return o;
    }

    glm::vec3 Ray::direction() const {
        return d; 
    }

    double Ray::time() const {
        return t; 
    }

    glm::vec3 Ray::at(double t) const {
        return o + static_cast<float>(t) * d; 
    }
}