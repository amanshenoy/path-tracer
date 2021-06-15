#include <iostream>

#include "../dependancies/glm/glm.hpp"
#include "../dependancies/colors.hpp"

#include "../utils/utils.hpp"

namespace utils {
    int show_progress_bar(double progress, int progress_bar_width, int limit){
        std::cout << BOLDCYAN << "[ RENDER ]" << GREEN << " |" << WHITE;  
        double pos = (progress_bar_width * progress) / (limit);  
        
        for (int i = 0; i < progress_bar_width; ++i) {
            int local_progress = static_cast<int>(((pos - static_cast<int>(pos)) * 10));
            if (i <= static_cast<int>(pos)) std::cout << "▇"; 
            else if (i == static_cast<int>(pos) + 1) std::cout << std::to_string(local_progress);
            else std::cout << " ";
        }
        std::cout.precision(4);
        std::cout << GREEN << "▎" << RESET << ((progress/limit) * 100.0) << "% \r";
        if (progress < limit - 1) std::cout.flush();
        else std::cout << "\n";
        return 0; 
    }

    int show_tree_building(double progress, int progress_bar_width, int limit){
        std::cout << BOLDCYAN << "[BVH TREE]" << GREEN << " |" << WHITE;  
        double pos = (progress_bar_width * progress) / (limit);  
        
        for (int i = 0; i < progress_bar_width; ++i) {
            int local_progress = static_cast<int>(((pos - static_cast<int>(pos)) * 10));
            if (i <= static_cast<int>(pos)) std::cout << "▇"; 
            else if (i == static_cast<int>(pos) + 1) std::cout << GREEN << "▇" << WHITE;
            else std::cout << " ";
        }
        std::cout.precision(4);
        std::cout << GREEN << "▎" << RESET << static_cast<int>(progress) + 2 << "/" << limit << "\r";
        if (progress < limit - 2) std::cout.flush();
        else std::cout << "\n";
        return 0; 
    }

    void hit_details::set_face(const core::Ray& ray, const glm::vec3& outward_normal) {
        front_face = glm::dot(ray.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }

    void ONB::build_from_w(const glm::vec3& normal, glm::vec3 tangent) {
        while (tangent == normal){
            tangent = glm::normalize(tangent + utils::sampler::random_in_hemisphere(tangent));  
        }
        axis[2] = glm::normalize(normal);
        axis[1] = glm::normalize(glm::cross(w(), tangent));
        axis[0] = glm::normalize(glm::cross(w(), v()));
        transform = glm::mat3(axis[1], axis[2], axis[0]);
    }
    void ONB::build_from_w(const glm::vec3& n) {
        axis[2] = glm::normalize(n);
        glm::vec3 a; 
        a = glm::vec3(0, 1, 0); 
        axis[1] = glm::normalize(glm::cross(w(), a));
        axis[0] = glm::normalize(glm::cross(w(), v()));
        transform = glm::mat3(axis[1], axis[2], axis[0]);
    }

    double clamp(double x, double min, double max) {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    bool near_zero(glm::vec3& e) {
        const auto s = 1e-8;
        return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
    }

    namespace sampler {
        double random_double() {
            // srand(500); 
            return rand() / (RAND_MAX + 1.0);
        }

        double random_double(double min, double max) {
            return min + (max-min)*random_double();
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
            return static_cast<int>(random_double(min, max+1));
        }

        glm::vec3 random_cosine_direction() {
            auto r1 = random_double();
            auto r2 = random_double();
            auto z = sqrt(1-r2);

            auto phi = 2*M_PI*r1;
            auto x = cos(phi)*sqrt(r2);
            auto y = sin(phi)*sqrt(r2);

            return glm::vec3(x, y, z);
        }

        glm::vec3 random_cosine_direction(float r1, float r2) {
            auto z = sqrt(1-r2);

            auto phi = 2*M_PI*r1;
            auto x = cos(phi)*sqrt(r2);
            auto y = sin(phi)*sqrt(r2);

            return glm::vec3(x, y, z);
        }
    }
}