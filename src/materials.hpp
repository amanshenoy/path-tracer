#pragma once

#include "dependancies/stb_image.h"
#include "utils.hpp"
#include "ray.hpp"

glm::vec3 reflect_ray(const glm::vec3& v, const glm::vec3& n);

glm::vec3 refract_ray(glm::vec3& ray_in, const glm::vec3& n, double ratio);

class Material {
public:
    virtual bool scatter(const Ray& ray_in, const hit_details& rec, glm::vec3& attenuation, Ray& scattered) = 0;
    virtual glm::vec3 emitted(double u, double v, const glm::vec3& p) = 0;
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

        virtual bool scatter(
            const Ray& r_in, const hit_details& rec, glm::vec3& attenuation, Ray& scattered
        ) override {
            return false;
        }

        virtual glm::vec3 emitted(double u, double v, const glm::vec3& p) override {
            return emit->value(u, v, p);
        }

    public:
        std::shared_ptr<Texture> emit;
};


class Metal : public Material {
public:
    Metal(const glm::vec3& a, double f) : albedo(a), fuzz(f < 1 ? f : 1){}
    virtual bool scatter(const Ray& ray_in, const hit_details& rec, glm::vec3& attenuation, Ray& scattered) override;
    inline virtual glm::vec3 emitted(double u, double v, const glm::vec3& p) override { return glm::vec3(0,0,0);}

public: 
    glm::vec3 albedo; 
    double fuzz;
};  

class Dielectric : public Material {
public:
    Dielectric(double refraction_index, double f) : ir(refraction_index), fuzz(f < 1 ? f : 1){}
    virtual bool scatter(const Ray& ray_in, const hit_details& rec, glm::vec3& attenuation, Ray& scattered) override;
    inline virtual glm::vec3 emitted(double u, double v, const glm::vec3& p) override { return glm::vec3(0,0,0);}

public: 
    double ir; 
    double fuzz;

private: 
    double reflectance(double cosine, double ref_idx);
};  

class Lambertian : public Material {
public:
    Lambertian(const glm::vec3& a) : albedo(std::make_shared<SolidColor>(a)){}
    Lambertian(std::shared_ptr<Texture> a) : albedo(a) {}

    virtual bool scatter(const Ray& ray_in, const hit_details& rec, glm::vec3& attenuation, Ray& scattered) override;
    inline virtual glm::vec3 emitted(double u, double v, const glm::vec3& p) override { return glm::vec3(0,0,0);}

public: 
    std::shared_ptr<Texture> albedo; 
}; 

class Isotropic : public Material {
public:
    Isotropic(glm::vec3 c) : albedo(std::make_shared<SolidColor>(c)) {}
    Isotropic(std::shared_ptr<Texture> a) : albedo(a) {}

    virtual bool scatter(const Ray& ray_in, const hit_details& rec, glm::vec3& attenuation, Ray& scattered) override;
    inline virtual glm::vec3 emitted(double u, double v, const glm::vec3& p) override { return glm::vec3(0,0,0);}

public:
    std::shared_ptr<Texture> albedo;
};