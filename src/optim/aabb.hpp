#pragma once
#include "../dependancies/glm/glm.hpp"
#include "../core/ray.hpp"

namespace optim {    
    class AABB{
    public:
        AABB() {};
        AABB(const glm::vec3& a, const glm::vec3& b): minimum(a), maximum(b) {}

        glm::vec3 min() {return minimum;}
        glm::vec3 max() {return maximum;}

        bool hit(const core::Ray& r, double t_min, double t_max);
    public:
        glm::vec3 minimum, maximum;  
    };

    AABB surrounding_box(AABB& box0, AABB& box1);
}