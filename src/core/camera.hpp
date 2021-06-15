#pragma once
#include "ray.hpp"

namespace core{
    class Camera{
    public:
        glm::vec3 eye, top_left, x_direction, y_direction;
        Camera(double fovy, double aspect_ratio, glm::vec3& lookfrom, glm::vec3& lookat, \
            glm::vec3& up, double aperture, double focus_distance, double start, double end); 

        Ray get_ray(double s, double t);

    private:
        glm::vec3 u, v, w; 
        double lens_radius;    
        double start_time, end_time;
    };
}