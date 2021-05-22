#include "dependancies/glm/glm.hpp"

#include "utils.hpp"
#include "camera.hpp"

Camera::Camera(double fovy, double aspect_ratio, glm::vec3& lookfrom, glm::vec3& lookat, \
    glm::vec3& up, double aperture, double focus_distance, double start, double end) {
    double theta = glm::radians(fovy); 
    double h = tan(theta / 2.0f);
    double viewport_height = 2.0f * h; 
    double viewport_width = viewport_height * aspect_ratio; 
    double focal_length = 1.0; 

    w = glm::normalize(lookfrom - lookat);
    u = glm::normalize(glm::cross(up, w));
    v = glm::cross(w, u);

    eye = lookfrom; 
    x_direction = static_cast<float>(focus_distance * viewport_width) * u; 
    y_direction = static_cast<float>(focus_distance * viewport_height) * v; 
    top_left = eye - x_direction/2.0f + y_direction/2.0f - static_cast<float>(focus_distance) * w; 

    lens_radius = aperture / 2.0f;

    start_time = start; 
    end_time = end; 
}

Ray Camera::get_ray(double s, double t){
    glm::vec3 rd = static_cast<float>(lens_radius) * random_in_unit_disk(); 
    glm::vec3 offset = u * rd.x + v * rd.y; 
    Ray ray(eye + offset, top_left + static_cast<float>(s)*x_direction - static_cast<float>(t)*y_direction - eye - offset, \
        random_double(start_time, end_time));
    return ray;
}