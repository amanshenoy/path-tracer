#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include <future>
#include <algorithm>

#include "dependancies/glm/glm.hpp"
#include "dependancies/FreeImage/FreeImage.h"

#include <cmath>
#include <limits>
#include <memory>
#include <random>

#include "utils/utils.hpp"
#include "utils/pdf.hpp"

#include "core/ray.hpp"
#include "core/objects.hpp"
#include "core/materials.hpp"
#include "core/camera.hpp"
#include "core/background.hpp"

#include "optim/bvh.hpp"

#include "dependancies/colors.hpp"

#include "scenes.hpp"

extern int progress_bar_width; 
extern int height, width; 

RGBQUAD write_maps(glm::vec3 map_color){
    RGBQUAD color; 
    double r, g, b;
    r = map_color.r; 
    g = map_color.g;
    b = map_color.b;

    r = static_cast<int>(utils::clamp(r, 0.0f, 0.999f) * 256.0f);  
    g = static_cast<int>(utils::clamp(g, 0.0f, 0.999f) * 256.0f); 
    b =  static_cast<int>(utils::clamp(b, 0.0f, 0.999f) * 256.0f);

    color.rgbRed = r;  
    color.rgbGreen = g; 
    color.rgbBlue =  b;

    return color; 
}

RGBQUAD write_color(glm::vec3 pixel_color, double samples_per_pixel){
    RGBQUAD color; 
    double r, g, b;
    r = pixel_color.r; 
    g = pixel_color.g;
    b = pixel_color.b;

    if (r != r) r = 0.0;
    if (g != g) g = 0.0;
    if (b != b) b = 0.0;

    double scale = 1.0f / samples_per_pixel;
    double gamma = 1.75f; 

    r = pow(scale * r, 1.0f / gamma);
    g = pow(scale * g, 1.0f / gamma);
    b = pow(scale * b, 1.0f / gamma);
    
    r = static_cast<int>(utils::clamp(r, 0.0f, 0.999f) * 256.0f);  
    g = static_cast<int>(utils::clamp(g, 0.0f, 0.999f) * 256.0f); 
    b =  static_cast<int>(utils::clamp(b, 0.0f, 0.999f) * 256.0f);

    color.rgbRed = r;  
    color.rgbGreen = g; 
    color.rgbBlue =  b;

    return color; 
}

void write_image(std::string image_path, FIBITMAP* image_object){
    if (FreeImage_Save(FIF_PNG, image_object, image_path.c_str(), 0)) 
        std::cout << BOLDCYAN << "[ STATUS ]" << RESET << " Image has been saved properly to " << image_path << std::endl; 
    else
        std::cout << BOLDCYAN << "[ STATUS ]" << RESET << " Path " << image_path << " does not exist!" << std::endl; 
}

glm::vec3 shoot_ray(const core::Ray& ray, core::Background& bg, core::Object& world, core::Scene& lights, int depth, bool first, glm::vec3& first_albedo, glm::vec3& first_normal){
    utils::hit_details rec;
    if (depth <= 0) return glm::vec3(0.0f);
    double infinity = std::numeric_limits<double>::infinity();

    if (!world.hit(ray, 0.005, infinity, rec)){
        if (first) first_normal = glm::vec3(0);
        if (first) first_albedo = glm::vec3(0); 
        return bg.get_color(ray); 
    }

    utils::scatter_details srec;
    glm::vec3 emitted = rec.mat_ptr->emitted(ray, rec, rec.u, rec.v, rec.p);

    srec.scattered = core::Ray(rec.p, glm::vec3(0, 0, 1), ray.time());
    srec.pdf_value = 1;

    if (!rec.mat_ptr -> bsdf(ray, rec, srec))
        return emitted;

    if (first){
        first_albedo = rec.mat_ptr -> get_albedo(); 
        first_normal = (-glm::normalize(rec.normal) + glm::vec3(1)) / 2.0f;
    }

   if (srec.is_specular) {
        return srec.attenuation
             * shoot_ray(srec.specular_ray, bg, world, lights, depth-1,false, first_albedo, first_normal);
    }

    // This is the code to be able to importance sample lights and has been commented out 
    //                  because it has not been reconfigured to work with the Disney BSDF

    // auto light_ptr = std::make_shared<ObjectPDF>(std::make_shared<Scene>(lights), rec.p);
    // srec.imp_sampler = light_ptr; 
    // MixturePDF p(srec.imp_sampler, srec.pdf_ptr, 1);

    // srec.scattered = Ray(rec.p, light_ptr -> generate(), ray.time());
    // srec.pdf_value = light_ptr -> value(srec.scattered.direction());
    
    core::Ray scattered = srec.scattered;
    auto pdf_val = srec.pdf_value;


    glm::vec3 albedo = srec.attenuation; 
    // return emitted + 
    //         albedo * static_cast<float>(rec.mat_ptr -> scattering_pdf(ray, rec, scattered)) 
    //                * shoot_ray(scattered, bg, world, lights, depth - 1, false, first_albedo, first_normal) / static_cast<float>(pdf_val);
    return emitted + 
            albedo * static_cast<float>(pdf_val) 
                   * shoot_ray(scattered, bg, world, lights, depth - 1, false, first_albedo, first_normal) / static_cast<float>(pdf_val);

}

int main(int argc, char* argv[]){

    FreeImage_Initialise(); 
    std::cout << BOLDCYAN << "[ STATUS ]" << RESET << " FreeImage version: " << FreeImage_GetVersion() << std::endl; 
    
    //////////////////////////////////////////////////////////////////////

    double aspect_ratio; 
    int height = 512;
    int width = 512;  
    int samples_per_pixel = 15;  
    int max_depth = 10;
 
    glm::vec3 lookfrom = glm::vec3(278.0f, 278.0f, -800.0f);
    glm::vec3 lookat = glm::vec3(278.0f, 278.0f, 0.0f); 
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    double fovy = 40.0; 
    float aperture = 0.0f; 
    float focal_distance = glm::length(lookfrom - lookat); 

    std::string scene = argv[1];

    core::Scene lights; 
    core::ImageBG background("res/backgrounds/blaubeuren_night_2k.hdr", 0.8, 0.4);
    core::Scene world = GetScene(aspect_ratio, lookfrom, lookat, height, width, fovy, scene, lights, aperture, focal_distance, background);
    core::Camera cam(fovy, aspect_ratio, lookfrom, lookat, up, aperture, focal_distance, 0.0f, 1.0f); 

    FIBITMAP* image  = FreeImage_Allocate(width, height, 24); 
    FIBITMAP* albedo_map  = FreeImage_Allocate(width, height, 24); 
    FIBITMAP* normal_map  = FreeImage_Allocate(width, height, 24); 

    std::string render_path = std::string("output/noisy_renders/") + scene + std::string(".png");
    std::string albedo_path = std::string("output/albedo_maps/") + scene + std::string(".png");
    std::string normal_path = std::string("output/normal_maps/") + scene + std::string(".png");

    //////////////////////////////////////////////////////////////////////

    srand(time(NULL));
    auto start = std::chrono::high_resolution_clock::now(); 
    double progress = 0.0f;

    #pragma omp parallel for
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            int progress_bar_width = 70; 

            #pragma omp critical
            utils::show_progress_bar(progress, progress_bar_width, width * height);
            progress += 1; 

            glm::vec3 pixel_color(0);
            glm::vec3 first_albedo, first_normal; 

            //////////////////////////////////////////////////////////////////////

            for (int s = 0; s < samples_per_pixel; ++s){
                double u = (static_cast<double>(j) + utils::sampler::random_double()) / width; 
                double v = (static_cast<double>(i) + utils::sampler::random_double()) / height; 
                core::Ray ray = cam.get_ray(u, v); 
                glm::vec3 sample_color = shoot_ray(ray, background, world, lights, max_depth, true, first_albedo, first_normal);
                pixel_color +=  sample_color;
            }

            //////////////////////////////////////////////////////////////////////

            auto color = std::make_shared<RGBQUAD>(write_color(pixel_color, samples_per_pixel));
            FreeImage_SetPixelColor(image, j, height - i, &*color);  

            auto albedo = std::make_shared<RGBQUAD>(write_maps(first_albedo)); 
            auto normal = std::make_shared<RGBQUAD>(write_maps(first_normal)); 
            FreeImage_SetPixelColor(albedo_map, j, height - i, &*albedo);  
            FreeImage_SetPixelColor(normal_map, j, height - i, &*normal);  
        }
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto render_time = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
    std::cout << BOLDCYAN << "[ STATUS ]" << RESET << " Total render time ~" << render_time.count() << " seconds" << std::endl;

    //////////////////////////////////////////////////////////////////////
    // Save renders and map images

    write_image(render_path, image); 
    write_image(albedo_path, albedo_map);
    write_image(normal_path, normal_map);

    FreeImage_DeInitialise();

    //////////////////////////////////////////////////////////////////////    
    return 0;
}
