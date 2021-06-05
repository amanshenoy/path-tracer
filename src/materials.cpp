#include <iostream>

#include "dependancies/colors.hpp"
#include "materials.hpp"

glm::vec3 reflect_ray(const glm::vec3& v, const glm::vec3& n){
    return v - 2 * glm::dot(v, n) * n;
}

glm::vec3 refract_ray(glm::vec3& uv, const glm::vec3& n, double ratio){
    double cos_theta = fmin(glm::dot(-uv, n), 1.0);
    glm::vec3 ray_out_perp = static_cast<float>(ratio) * (uv + static_cast<float>(cos_theta) * n);
    glm::vec3 ray_out_parallel = static_cast<float>(-sqrt(fabs(1.0f - pow(glm::length(ray_out_perp), 2)))) * n;
    return ray_out_perp + ray_out_parallel;  
}

bool Metal::bsdf(const Ray& ray_in, const hit_details& rec, glm::vec3& attenuation, Ray& scattered) {
    glm::vec3 reflected = reflect_ray(glm::normalize(ray_in.direction()), rec.normal);
    scattered = Ray(rec.p, reflected + static_cast<float>(fuzz) * random_in_unit_sphere(), ray_in.time());
    attenuation = albedo;
    return (glm::dot(scattered.direction(), rec.normal) > 0); 
}

bool Dielectric::bsdf(const Ray& ray_in, const hit_details& rec, glm::vec3& attenuation, Ray& scattered) {
    attenuation = glm::vec3(1.0);
    float refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

    glm::vec3 unit_direction = glm::normalize(ray_in.direction());

    double cos_theta = fmin(glm::dot(-unit_direction, rec.normal), 1.0);
    double sin_theta = sqrt(1.0 - cos_theta*cos_theta);
    bool cannot_refract = refraction_ratio * sin_theta > 1.0;
    glm::vec3 direction; 

    if ((cannot_refract) || reflectance(cos_theta, refraction_ratio) > random_double())
        direction = reflect_ray(unit_direction, rec.normal);
    else
        direction = refract_ray(unit_direction, rec.normal, refraction_ratio);

    scattered = Ray(rec.p, direction + static_cast<float>(fuzz) * random_in_unit_sphere(), ray_in.time());
    return true;         
}

double Dielectric::reflectance(double cosine, double ref_idx) {
    // Schlick approximation  
    double r0 = (1 - ref_idx) / (1 + ref_idx); 
    r0 = r0 * r0; 
    return r0 + (1 - r0) * pow(1 - cosine, 5);
}

bool Lambertian::bsdf(const Ray& ray_in, const hit_details& rec, glm::vec3& attenuation, Ray& scattered) {
    glm::vec3 scatter_dirn = random_in_hemisphere(rec.normal);

    if (near_zero(scatter_dirn))
        scatter_dirn = rec.normal;

    scattered = Ray(rec.p, scatter_dirn, ray_in.time());
    attenuation = albedo -> value(rec.u, rec.v, rec.p);
    return true; 
}

bool Glossy::bsdf(const Ray& ray_in, const hit_details& rec, glm::vec3& attenuation, Ray& scattered) {
    glm::vec3 reflected = reflect_ray(glm::normalize(ray_in.direction()), rec.normal);
    glm::vec3 scatter_dirn = reflected + static_cast<float>(sharpness) * random_in_hemisphere(reflected);
    scattered = Ray(rec.p, scatter_dirn, ray_in.time());
    attenuation = albedo -> value(rec.u, rec.v, rec.p);
    return true; 
}

bool Isotropic::bsdf(const Ray& ray_in, const hit_details& rec, glm::vec3& attenuation, Ray& scattered) {
    scattered = Ray(rec.p, random_in_unit_sphere(), ray_in.time());
    attenuation = albedo->value(rec.u, rec.v, rec.p);
    return true;
}

glm::vec3 CheckeredTexture::value(double u, double v, const glm::vec3& p) {
    auto sines = sin(10 * p.x) * sin(10 * p.y) * sin(10 * p.z);
    if (sines < 0)
        return odd->value(u, v, p);
    else
        return even->value(u, v, p);
}

ImageTexture::ImageTexture(const char* filename) {
    int components_per_pixel = bytes_per_pixel;
    data = stbi_load(filename, &width, &height, &components_per_pixel, components_per_pixel);
    if (!data){
        std::cerr << BOLDCYAN << "[ERROR]" << RESET << " Could not load image file " << filename << '\n';
    }
    bytes_per_scanline = bytes_per_pixel * width; 
}

glm::vec3 ImageTexture::value(double u, double v, const glm::vec3& p) {
    if (data == nullptr) return glm::vec3(0.0f, 1.0f, 1.0f);
    u = clamp(u, 0.0f, 1.0f);
    v = 1.0f - clamp(v, 0.0f, 1.0f);

    int i = static_cast<int>(u * width); 
    int j = static_cast<int>(v * height);

    if (i >= width) i = width - 1; 
    if (j >= width) j = height - 1; 

    const double color_scale = 1.0f / 255.0f;
    auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;

    return glm::vec3(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
}
