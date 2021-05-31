#include <iostream>
#include "aabb.hpp"

bool AABB::hit(const Ray& r, double t_min, double t_max) 
{
    for (int a = 0; a < 3; a++) {
        auto invD = 1.0f / r.direction()[a];
        auto t0 = (min()[a] - r.origin()[a]) * invD;
        auto t1 = (max()[a] - r.origin()[a]) * invD;
        if (invD < 0.0f)
            std::swap(t0, t1);
        t_min = t0 > t_min ? t0 : t_min;
        t_max = t1 < t_max ? t1 : t_max;
        if (t_max <= t_min)
            return false;
    }
    return true;    
}

AABB surrounding_box(AABB& box0, AABB& box1) {
    glm::vec3 small(fmin(box0.min().x, box1.min().x),
                    fmin(box0.min().y, box1.min().y),
                    fmin(box0.min().z, box1.min().z));

    glm::vec3 big(fmax(box0.max().x, box1.max().x),
                  fmax(box0.max().y, box1.max().y),
                  fmax(box0.max().z, box1.max().z));

    // std::cout << fmin(box0.min().x, box1.min().x)<< " " <<
    //                 fmin(box0.min().y, box1.min().y)<< " " <<
    //                 fmin(box0.min().z, box1.min().z)<<" ";

    // std::cout << fmax(box0.max().x, box1.max().x) << " " <<
    //               fmax(box0.max().y, box1.max().y)<<" " <<
    //               fmax(box0.max().z, box1.max().z) << std::endl;

    return AABB(small,big);
}