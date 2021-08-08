#pragma once

#include "../dependancies/stb/stb_image.h"
#include "../utils/utils.hpp"
#include "ray.hpp"

#include "../shaders/bsdf.hpp"

namespace core {
    glm::vec3 reflect_ray(const glm::vec3& v, const glm::vec3& n);

    glm::vec3 refract_ray(glm::vec3& ray_in, const glm::vec3& n, double ratio);

    class Material {
    public:
        virtual bool bsdf(const Ray& ray_in, const utils::hit_details& rec, utils::scatter_details& srec) {return false;};
        virtual double scattering_pdf(const Ray& ray_in, const utils::hit_details& rec, const Ray& scattered) {return 0;}
        virtual glm::vec3 emitted(const Ray& ray_in, const utils::hit_details& rec, double u, double v, const glm::vec3& p) {return glm::vec3(0);};
        virtual glm::vec3 get_albedo() {return alb;}
    public: 
        glm::vec3 alb; 
    };

    class Texture {
        public:
            virtual glm::vec3 value(double u, double v, const glm::vec3& p) = 0;
    };

    class SolidColor : public Texture{
        public:
            SolidColor() {}
            SolidColor(glm::vec3 c) : color_value(c) {}

            SolidColor(double red, double green, double blue)
            : SolidColor(glm::vec3(red, green, blue)) {}

            virtual glm::vec3 value(double u, double v, const glm::vec3& p) override {
                return color_value;
            }

        private:
            glm::vec3 color_value;
    };

    class ImageTexture : public Texture {
    public:
        const static int bytes_per_pixel = 3; 
        ImageTexture() : data(nullptr), width(0), height(0), bytes_per_scanline(0) {}
        ImageTexture(const char* filename);
        ~ImageTexture(){
            delete data; 
        }
        virtual glm::vec3 value(double u, double v, const glm::vec3& p) override;
    private:
        unsigned char *data; 
        int width, height; 
        int bytes_per_scanline; 
    };

    class CheckeredTexture: public Texture {
        public:
            CheckeredTexture() {}

            CheckeredTexture(std::shared_ptr<Texture> _even, std::shared_ptr<Texture> _odd)
                : even(_even), odd(_odd) {}

            CheckeredTexture(glm::vec3 c1, glm::vec3 c2)
                : even(std::make_shared<SolidColor>(c1)) , odd(std::make_shared<SolidColor>(c2)) {}

            virtual glm::vec3 value(double u, double v, const glm::vec3& p) override;

        public:
            std::shared_ptr<Texture> odd, even;
    };

    class Emitter : public Material {
        public:
            Emitter(std::shared_ptr<Texture> a) : emit(a) {}
            Emitter(glm::vec3 c) : emit(std::make_shared<SolidColor>(c)) {}

            virtual bool bsdf(const Ray& r_in, const utils::hit_details& rec, utils::scatter_details& srec) override {
                return false;
            }
            virtual glm::vec3 emitted(const Ray& ray_in, const utils::hit_details& rec,double u, double v, const glm::vec3& p) override {
                if (rec.front_face) return emit->value(u, v, p);
                else return glm::vec3(0); 
            }

        public:
            std::shared_ptr<Texture> emit;
            glm::vec3 alb;
    };

    class Metal : public Material {
    public:
        Metal(const glm::vec3& a, double f) : alb(a), fuzz(f < 1 ? f : 1){}
        virtual bool bsdf(const Ray& ray_in, const utils::hit_details& rec, utils::scatter_details& srec) override;
        inline virtual glm::vec3 emitted(const Ray& ray_in, const utils::hit_details& rec, double u, double v, const glm::vec3& p) override { return glm::vec3(0,0,0);}
        inline glm::vec3 get_albedo() override {return alb;}
    public: 
        glm::vec3 alb; 
        double fuzz;
    };  

    class Dielectric : public Material {
    public:
        Dielectric(double refraction_index, double f) : ir(refraction_index), fuzz(f < 1 ? f : 1), alb(glm::vec3(0)){}
        virtual bool bsdf(const Ray& ray_in, const utils::hit_details& rec, utils::scatter_details& srec) override;
        inline virtual glm::vec3 emitted(const Ray& ray_in, const utils::hit_details& rec, double u, double v, const glm::vec3& p) override { return glm::vec3(0,0,0);}
        inline glm::vec3 get_albedo() override {return alb;}
    public: 
        double ir; 
        double fuzz;
        glm::vec3 alb; 
    private: 
        double reflectance(double cosine, double ref_idx);
    };  

    class Lambertian : public Material {
    public:
        Lambertian(const glm::vec3& a) : albedo(std::make_shared<SolidColor>(a)), alb(a){}
        Lambertian(std::shared_ptr<Texture> a) : albedo(a) {}

        virtual bool bsdf(const Ray& ray_in, const utils::hit_details& rec, utils::scatter_details& srec) override;
        virtual double scattering_pdf(const Ray& ray_in, const utils::hit_details& rec, const Ray& scattered) override; 
        inline virtual glm::vec3 emitted(const Ray& ray_in, const utils::hit_details& rec,double u, double v, const glm::vec3& p) override { return glm::vec3(0);}
        inline glm::vec3 get_albedo() override {return alb;}
    public: 
        std::shared_ptr<Texture> albedo; 
        glm::vec3 alb; 
    }; 

    class Isotropic : public Material {
    public:
        Isotropic(glm::vec3 c) : albedo(std::make_shared<SolidColor>(c)), alb(c) {}
        Isotropic(std::shared_ptr<Texture> a) : albedo(a) {}

        virtual bool bsdf(const Ray& ray_in, const utils::hit_details& rec, utils::scatter_details& srec) override;
        inline virtual double scattering_pdf(const Ray& ray_in, const utils::hit_details& rec, const Ray& scattered) override {return 0;}; 
        inline virtual glm::vec3 emitted(const Ray& ray_in, const utils::hit_details& rec,double u, double v, const glm::vec3& p) override { return glm::vec3(0);}
        inline glm::vec3 get_albedo() override {return alb;}

    public:
        glm::vec3 alb; 
        std::shared_ptr<Texture> albedo;
    };

    class Disney : public Material {
    public:
        Disney(const glm::vec3& a, float specular, float diffuse, float clearcoat, float transmission, bool thin);

        bsdf::SurfaceParameters get_surface_point(const utils::hit_details& rec); 
        virtual bool bsdf(const Ray& ray_in, const utils::hit_details& rec, utils::scatter_details& srec) override;
        inline virtual double scattering_pdf(const Ray& ray_in, const utils::hit_details& rec, const Ray& scattered) override {return 0;}; 
        inline virtual glm::vec3 emitted(const Ray& ray_in, const utils::hit_details& rec,double u, double v, const glm::vec3& p) override { return glm::vec3(0);}
        inline glm::vec3 get_albedo() override {return alb;}
    public: 
        glm::vec3 alb; 
        bsdf::SurfaceParameters surface_params;
        float specular, diffuse, clearcoat, transmission;
        bool thin; 
    }; 
}