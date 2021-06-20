#pragma once
#include "../dependancies/glm/glm.hpp"

#include <memory>

#include "../core/ray.hpp"

namespace core {class Material;} 

namespace utils {
    class hit_details {
    public:
        glm::vec3 p;
        glm::vec3 normal; 
        std::shared_ptr<core::Material> mat_ptr; 
        double t;
        double u;
        double v;
        bool front_face; 

        glm::vec3 first_albedo; 
        glm::vec3 first_normal; 

        void set_face(const core::Ray& ray, const glm::vec3& outward_normal); 
    };

    class PDF; 
    struct scatter_details {
        core::Ray specular_ray;
        bool is_specular;
        glm::vec3 attenuation;
        std::shared_ptr<PDF> pdf_ptr;
        std::shared_ptr<PDF> imp_sampler;

        core::Ray scattered; 
        double pdf_value;
    };

    class LocalBasis {
        public:
            LocalBasis() {}

            inline glm::vec3 operator[](int i) const { return axis[i]; }

            glm::vec3 u() const { return axis[0]; }
            glm::vec3 v() const { return axis[1]; }
            glm::vec3 w() const { return axis[2]; }

            glm::vec3 local(float a, float b, float c) const {
                return a*u() + b*v() + c*w();
            }

            glm::vec3 local(const glm::vec3& a) const {
                return a.x*u() + a.y*v() + a.z*w();
            }

            void build_from_w(const glm::vec3& normal, glm::vec3 tangent);
            void build_from_w(const glm::vec3& n);

        public:
            glm::vec3 axis[3];
            glm::mat3 transform; 
    };

    int show_progress_bar(double progress, int progress_bar_width, int limit);

    int show_tree_building(double progress, int progress_bar_width, int limit);

    double clamp(double x, double min, double max);

    bool near_zero(glm::vec3& e);
    
    namespace sampler {
        double random_double();

        double random_double(double min, double max);

        int random_int(int min, int max);

        glm::vec3 random_in_unit_sphere();

        glm::vec3 random_unit_vector();

        glm::vec3 random_in_unit_disk();

        glm::vec3 random_in_hemisphere(const glm::vec3& normal);

        glm::vec3 random_cosine_direction();

        glm::vec3 random_cosine_direction(float r1, float r2);
    }
}