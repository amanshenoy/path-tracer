#include <iostream>

#include "../dependancies/colors.hpp"
#include "materials.hpp"
#include "../utils/pdf.hpp"
#include "../shaders/bsdf.hpp"

namespace core {
    glm::vec3 reflect_ray(const glm::vec3& v, const glm::vec3& n){
        return v - 2 * glm::dot(v, n) * n;
    }

    glm::vec3 refract_ray(glm::vec3& uv, const glm::vec3& n, double ratio){
        double cos_theta = fmin(glm::dot(-uv, n), 1.0);
        glm::vec3 ray_out_perp = static_cast<float>(ratio) * (uv + static_cast<float>(cos_theta) * n);
        glm::vec3 ray_out_parallel = static_cast<float>(-sqrt(fabs(1.0f - pow(glm::length(ray_out_perp), 2)))) * n;
        return ray_out_perp + ray_out_parallel;  
    }


    bool Metal::bsdf(const Ray& ray_in, const utils::hit_details& rec, utils::scatter_details& srec) {
        glm::vec3 reflected = reflect(glm::normalize(ray_in.direction()), rec.normal);
        srec.specular_ray = Ray(rec.p, reflected+static_cast<float>(fuzz)* utils::sampler::random_in_unit_sphere(), ray_in.time());
        srec.attenuation = alb;
        srec.is_specular = true;
        srec.pdf_ptr = 0;
        return true;    
    }

    bool Dielectric::bsdf(const Ray& ray_in, const utils::hit_details& rec, utils::scatter_details& srec) {
        srec.is_specular = true;
        srec.pdf_ptr = nullptr;
        srec.attenuation = glm::vec3(1.0, 1.0, 1.0);
        float refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

        glm::vec3 unit_direction = glm::normalize(ray_in.direction());

        double cos_theta = fmin(glm::dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta*cos_theta);
        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        glm::vec3 direction; 

        if ((cannot_refract) || reflectance(cos_theta, refraction_ratio) > utils::sampler::random_double())
            direction = reflect_ray(unit_direction, rec.normal);
        else
            direction = refract_ray(unit_direction, rec.normal, refraction_ratio);

        srec.specular_ray = Ray(rec.p, direction +static_cast<float>(fuzz) * utils::sampler::random_in_unit_sphere(), ray_in.time());
        return true;  
    }

    double Dielectric::reflectance(double cosine, double ref_idx) {
        // Schlick approximation  for the Fresnel term
        double r0 = (1 - ref_idx) / (1 + ref_idx); 
        r0 = r0 * r0; 
        return r0 + (1 - r0) * pow(1 - cosine, 5);
    }

    bool Lambertian::bsdf(const Ray& ray_in, const utils::hit_details& rec, utils::scatter_details& srec) {
        srec.is_specular = false; 
        srec.attenuation = albedo -> value(rec.u, rec.v, rec.p);
        auto p =  std::make_shared<utils::CosinePDF>(rec.normal); 
        srec.scattered = Ray(rec.p, p->generate(), ray_in.time());
        srec.pdf_value = p->value(srec.scattered.direction());
        return true; 
    }

    double Lambertian::scattering_pdf(const Ray& ray_in, const utils::hit_details& rec, const Ray& scattered) {
        auto cosine = glm::dot(rec.normal, glm::normalize(scattered.direction()));
        return cosine < 0 ? 0 : cosine / M_PI; 
    }

    bool Isotropic::bsdf(const Ray& ray_in, const utils::hit_details& rec, utils::scatter_details& srec) {
        srec.scattered = Ray(rec.p, utils::sampler::random_in_unit_sphere(), ray_in.time());
        srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
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
        u = utils::clamp(u, 0.0f, 1.0f);
        v = 1.0f - utils::clamp(v, 0.0f, 1.0f);

        int i = static_cast<int>(u * width); 
        int j = static_cast<int>(v * height);

        if (i >= width) i = width - 1; 
        if (j >= width) j = height - 1; 

        const double color_scale = 1.0f / 255.0f;
        auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;

        return glm::vec3(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
    }

    Disney::Disney(const glm::vec3& a, float specular, float diffuse, float clearcoat, float transmission, bool thin){
        this -> alb = a; 
        this -> specular = specular; 
        this -> diffuse = diffuse; 
        this -> clearcoat = clearcoat; 
        this -> transmission = transmission; 
        this -> thin = thin;

        surface_params.baseColor = alb; 
        surface_params.transmittanceColor = glm::vec3(0.5);
        surface_params.sheen = 0.5;
        surface_params.sheenTint= 0.5;
        surface_params.diffTrans= 0;
        surface_params.flatness= 0.5;
        surface_params.roughness= 0.5;
        surface_params.scatterDistance= 0.5;

        surface_params.clearcoat= 0.5;
        surface_params.clearcoatGloss=0.5;

        surface_params.anisotropic= 0.5;
        surface_params.specularTint= 0.5;
        surface_params.metallic = 0;
        surface_params.specTrans= 0.5;

        surface_params.ior= 1.5;
        surface_params.relativeIOR = 1.5; 
    }

    bsdf::SurfaceParameters Disney::get_surface_point(const utils::hit_details& rec){
        bsdf::SurfaceParameters surface;
        surface.position = rec.p; 
        surface.basis.build_from_w(rec.normal, rec.p); 
        surface.worldToTangent = surface.basis.transform;

        surface.baseColor = surface_params.baseColor; 
        surface.transmittanceColor = surface_params.transmittanceColor;
        surface.sheen = surface_params.sheen;
        surface.sheenTint = surface_params.sheenTint;
        surface.diffTrans = surface_params.diffTrans;
        surface.flatness = surface_params.flatness;
        surface.roughness = surface_params.roughness;
        surface.scatterDistance = surface_params.scatterDistance;

        surface.clearcoat = surface_params.clearcoat;
        surface.clearcoatGloss =surface_params.clearcoatGloss;

        surface.anisotropic = surface_params.anisotropic;
        surface.specularTint = surface_params.specularTint;
        surface.metallic = surface_params.metallic;
        surface.specTrans = surface_params.specTrans;

        surface.ior = surface_params.ior;
        surface.relativeIOR = surface_params.relativeIOR;

        return surface;
    }

    bool Disney::bsdf(const core::Ray& ray_in, const utils::hit_details& rec, utils::scatter_details& srec) {
        // The disney bsdf material has been poorly implemented and the clearcoat and transmission lobes are    
                    // definitely faulty. 
        // I would imagine this material to be a little bit off from what you would actually expect the 
                    // disney bsdf to be 
        // In case anyone happens to be reading this, DO NOT take help from this class implementation 
        // Would recommend the implementation and corresponding blog by https://github.com/schuttejoe/Selas
                    // or the implementation by https://github.com/mmacklin/tinsel. 

        srec.is_specular = false; 

        auto surface = get_surface_point(rec);
        bsdf::BsdfSample sample;

        bool adds_up = fabs(1.0 - specular - diffuse - clearcoat - transmission) < 0.05 ; 
        assert (adds_up); 

        auto choose_ray = utils::sampler::random_double();
        
        if (choose_ray < specular){ 
            srec.is_specular = true; 
            srec.attenuation = bsdf::CalculateTint(alb); 
            srec.specular_ray = core::Ray(rec.p, reflect_ray(ray_in.direction(), rec.normal), ray_in.time()); 
            srec.pdf_value = 1 - surface.metallic; 
        }

        else if (specular <= choose_ray && choose_ray < diffuse + specular){
            bool sample_diffuse = bsdf::SampleDisneyDiffuse(surface, ray_in.direction(), this -> thin, sample); 
            if (sample_diffuse){
                srec.attenuation = sample.reflectance; 
                srec.scattered = core::Ray(rec.p, sample.wi, ray_in.time()); 
                srec.pdf_value = sample.forwardPdfW; 
            }
        }

        else if (specular + diffuse <= choose_ray && choose_ray < specular + diffuse + clearcoat){
            bool sample_clearcoat = SampleDisneyClearcoat(surface, ray_in.direction(), sample); 
            if (sample_clearcoat){
                srec.scattered = Ray(rec.p, sample.wi, ray_in.time()); 
                srec.pdf_value = sample.forwardPdfW; 
                srec.attenuation = sample.reflectance; 
            }
        }

        else if (specular + diffuse + clearcoat <= choose_ray && choose_ray < diffuse + specular + clearcoat + transmission){            
            bool sample_transmit = SampleDisneySpecTransmission(surface, ray_in.direction(), this -> thin, sample); 
            if (sample_transmit){
                srec.attenuation = sample.reflectance; 
                srec.scattered = Ray(rec.p, sample.wi, ray_in.time()); 
                srec.pdf_value = sample.forwardPdfW; 
            }
        }
        return true; 
    }
}