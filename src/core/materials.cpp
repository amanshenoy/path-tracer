#include <iostream>

#include "../dependancies/colors.hpp"
#include "materials.hpp"
#include "../utils/pdf.hpp"

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


    bool Metal::bsdf(const Ray& ray_in, const utils::hit_details& rec, utils::scatter_record& srec) {
        // glm::vec3 reflected = reflect_ray(glm::normalize(ray_in.direction()), rec.normal);
        // scattered = Ray(rec.p, reflected + static_cast<float>(fuzz) * random_in_unit_sphere(), ray_in.time());
        // attenuation = albedo;
        // return (glm::dot(scattered.direction(), rec.normal) > 0); 
        glm::vec3 reflected = reflect(glm::normalize(ray_in.direction()), rec.normal);
        srec.specular_ray = Ray(rec.p, reflected+static_cast<float>(fuzz)* utils::sampler::random_in_unit_sphere(), ray_in.time());
        srec.attenuation = alb;
        srec.is_specular = true;
        srec.pdf_ptr = 0;
        return true;    
    }

    bool Dielectric::bsdf(const Ray& ray_in, const utils::hit_details& rec, utils::scatter_record& srec) {
        // attenuation = glm::vec3(1.0);
        // float refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

        // glm::vec3 unit_direction = glm::normalize(ray_in.direction());

        // double cos_theta = fmin(glm::dot(-unit_direction, rec.normal), 1.0);
        // double sin_theta = sqrt(1.0 - cos_theta*cos_theta);
        // bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        // glm::vec3 direction; 

        // if ((cannot_refract) || reflectance(cos_theta, refraction_ratio) > random_double())
        //     direction = reflect_ray(unit_direction, rec.normal);
        // else
        //     direction = refract_ray(unit_direction, rec.normal, refraction_ratio);

        // scattered = Ray(rec.p, direction + static_cast<float>(fuzz) * random_in_unit_sphere(), ray_in.time());
        // return true;       

        srec.is_specular = true;
        srec.pdf_ptr = nullptr;
        srec.attenuation = glm::vec3(1.0, 1.0, 1.0);
        // double refraction_ratio = rec.front_face ? (1.0/ir) : ir;
        // ...
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

    bool Lambertian::bsdf(const Ray& ray_in, const utils::hit_details& rec, utils::scatter_record& srec) {
        // glm::vec3 direction = rec.normal + random_unit_vector();
        
        // if (near_zero(direction))
        //     direction = rec.normal;
        // ONB uvw;
        // uvw.build_from_w(rec.normal); 
        // auto direction = uvw.local(random_cosine_direction()); 
        // scattered = Ray(rec.p, glm::normalize(direction), ray_in.time());
        // attenuation = albedo -> value(rec.u, rec.v, rec.p);
        // // pdf = glm::dot(rec.normal, scattered.direction()) / M_PI; 
        // // pdf = 0.5 / M_PI; 
        // pdf = glm::dot(uvw.w(), scattered.direction()) / M_PI; 
        srec.is_specular = false; 
        srec.attenuation = albedo -> value(rec.u, rec.v, rec.p);
        // auto cosine_pdf = CosinePDF(rec.normal);
        auto p =  std::make_shared<utils::CosinePDF>(rec.normal); 
        srec.scattered = Ray(rec.p, p->generate(), ray_in.time());
        srec.pdf_value = p->value(srec.scattered.direction());
        return true; 
    }

    double Lambertian::scattering_pdf(const Ray& ray_in, const utils::hit_details& rec, const Ray& scattered) 
    {
        auto cosine = glm::dot(rec.normal, glm::normalize(scattered.direction()));
        return cosine < 0 ? 0 : cosine / M_PI; 
    }

    // bool Glossy::bsdf(const Ray& ray_in, const hit_details& rec, glm::vec3& attenuation, Ray& scattered) {
    //     glm::vec3 reflected = reflect_ray(glm::normalize(ray_in.direction()), rec.normal);
    //     glm::vec3 scatter_dirn = reflected + static_cast<float>(sharpness) * random_in_hemisphere(reflected);
    //     scattered = Ray(rec.p, scatter_dirn, ray_in.time());
    //     attenuation = albedo -> value(rec.u, rec.v, rec.p);
    //     return true; 
    // }

    // bool Isotropic::bsdf(const Ray& ray_in, const hit_details& rec, glm::vec3& attenuation, Ray& scattered) {
    //     scattered = Ray(rec.p, random_in_unit_sphere(), ray_in.time());
    //     attenuation = albedo->value(rec.u, rec.v, rec.p);
    //     return true;
    // }

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

    #include "../shaders/bsdf.hpp"



    bool Disney::bsdf(const Ray& ray_in, const utils::hit_details& rec, utils::scatter_record& srec) {
        srec.is_specular = false; 
        // srec.attenuation = albedo -> value(rec.u, rec.v, rec.p);
        
        // srec.pdf_ptr =  std::make_shared<CosinePDF>(rec.normal); 

        auto p = std::make_shared<utils::CosinePDF>(rec.normal);
        // auto cosine_pdf = std::make_shared<CosinePDF>(rec.normal);
        // auto light_ptr = std::make_shared<ObjectPDF>(std::make_shared<Scene>(srec.lights), rec.p);
        
        // MixturePDF p(light_ptr, cosine_pdf, 0.5);
        
        // auto light_sampler = srec.imp_sampler -> generate(); 
        // srec.pdf_value = p->value(srec.scattered.direction());
        // std::cout << "here" << std::endl;

        bsdf::SurfaceParameters my_surface;
        bsdf::BsdfSample sample;
        my_surface.position = rec.p; 
        my_surface.basis.build_from_w(rec.normal, rec.p); 
        my_surface.worldToTangent = my_surface.basis.transform;
        auto spring_green = glm::vec3(0, 255.0f/255.0f, 127.0f/255.0f); 
        my_surface.baseColor = alb;
        my_surface.transmittanceColor = glm::vec3(1);
        my_surface.sheen = 0.2;
        my_surface.sheenTint= 0.5;
        // my_surface.diffTrans= 0.2;
        my_surface.flatness= 0;
        my_surface.roughness= 0;
        my_surface.scatterDistance= 1;

        my_surface.clearcoat= 0.5;
        my_surface.clearcoatGloss=0.5;

        // my_surface.anisotropic= 0.5;
        // my_surface.specularTint= 0.5;
        // my_surface.metallic= 0.5;
        // my_surface.specTrans= 0.5;

        // my_surface.ior= 0.5;

        // bool sample_clearcoat = SampleDisneyClearcoat(my_surface, ray_in.direction(), sample); 
        // if (sample_clearcoat){
        //     srec.scattered = Ray(rec.p, sample.wi, ray_in.time()); 
        //     srec.pdf_value = sample.forwardPdfW; 
        //     srec.attenuation = sample.reflectance; 
        // }
        bool sample_diffuse = SampleDisneyDiffuse(my_surface, ray_in.direction(), true, sample); 
        if (sample_diffuse){
            srec.attenuation = sample.reflectance; 
            srec.scattered = Ray(rec.p, sample.wi, ray_in.time()); 
            srec.pdf_value = sample.forwardPdfW; 
        }
        // bool sample_transmit = SampleDisneySpecTransmission(my_surface, ray_in.direction(), true, sample); 
        // if (sample_transmit){
        //     srec.attenuation = sample.reflectance; 
        //     srec.scattered = Ray(rec.p, sample.wi, ray_in.time()); 
        //     srec.pdf_value = sample.forwardPdfW; 
        // }


        //     std::cout << "Something is off" << std::endl ;
        // else std::cout << "Haha all well" << std::endl; 
        return true; 
    }

    double Disney::scattering_pdf(const Ray& ray_in, const utils::hit_details& rec, const Ray& scattered) {
        // auto cosine = glm::dot(rec.normal, glm::normalize(scattered.direction()));
        // return cosine < 0 ? 0 : cosine / M_PI; 
        return 0;
    }
}