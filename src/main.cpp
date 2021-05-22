#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include <future>

#include "dependancies/glm/glm.hpp"
#include "dependancies/FreeImage/FreeImage.h"

#include <cmath>
#include <limits>
#include <memory>
#include <random>

#include "ray.hpp"
#include "utils.hpp"
#include "objects.hpp"
#include "materials.hpp"
#include "camera.hpp"
#include "bvh.hpp"

#include "dependancies/colors.hpp"
#include "scenes.hpp"

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

RGBQUAD write_maps(glm::vec3 map_color){
    RGBQUAD color; 
    double r, g, b;
    r = map_color.r; 
    g = map_color.g;
    b = map_color.b;

    r = static_cast<int>(clamp(r, 0.0f, 0.999f) * 256.0f);  
    g = static_cast<int>(clamp(g, 0.0f, 0.999f) * 256.0f); 
    b =  static_cast<int>(clamp(b, 0.0f, 0.999f) * 256.0f);

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

    double scale = 1.0f / samples_per_pixel;
    double gamma = 1.75f; 

    r = pow(scale * r, 1.0f / gamma);
    g = pow(scale * g, 1.0f / gamma);
    b = pow(scale * b, 1.0f / gamma);
    
    r = static_cast<int>(clamp(r, 0.0f, 0.999f) * 256.0f);  
    g = static_cast<int>(clamp(g, 0.0f, 0.999f) * 256.0f); 
    b =  static_cast<int>(clamp(b, 0.0f, 0.999f) * 256.0f);

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

glm::vec3 shoot_ray(const Ray& ray, const glm::vec3& bg, Object& world, int depth, bool first, glm::vec3& first_albedo, glm::vec3& first_normal){
    hit_details rec;   
    if (depth <= 0) return glm::vec3(0.0f);
    double infinity = std::numeric_limits<double>::infinity();

    if (!world.hit(ray, 0.005, infinity, rec))
        return bg;

    Ray scattered; 
    glm::vec3 attenuation; 
    glm::vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat_ptr -> scatter(ray, rec, attenuation, scattered))
        return emitted;

    if (first) {
        first_albedo = attenuation; 
        first_normal = glm::normalize(rec.normal);
    }
    return emitted + attenuation * shoot_ray(scattered, bg, world, depth - 1, false, first_albedo, first_normal);
}

int main(int argc, char* argv[]){

    FreeImage_Initialise(); 
    std::cout << BOLDCYAN << "[ STATUS ]" << RESET << " FreeImage version: " << FreeImage_GetVersion() << std::endl; 
    
    //////////////////////////////////////////////////////////////////////
    // Create scene and camera, and allocate memory for images

    double aspect_ratio; 
    int height = 360; 
    int width; 
    int samples_per_pixel = 1; 
    int max_depth = 10;

    glm::vec3 lookfrom = glm::vec3(278.0f, 278.0f, -800.0f);
    glm::vec3 lookat = glm::vec3(278.0f, 278.0f, 0.0f); 
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    double fovy = 40.0; 

    std::string scene = argv[1]; 
    Scene world = GetScene(aspect_ratio, lookfrom, lookat, height, width, fovy, scene);
    Camera cam(fovy, aspect_ratio, lookfrom, lookat, up, 0.0f, glm::length(lookfrom - lookat), 0.0f, 1.0f); 

    FIBITMAP* image  = FreeImage_Allocate(width, height, 24); 
    FIBITMAP* albedo_map  = FreeImage_Allocate(width, height, 24); 
    FIBITMAP* normal_map  = FreeImage_Allocate(width, height, 24); 

    std::string render_path = std::string("output/noisy_renders/") + scene + std::string(".png");
    std::string albedo_path = std::string("output/albedo_maps/") + scene + std::string(".png");
    std::string normal_path = std::string("output/normal_maps/") + scene + std::string(".png");

    glm::vec3 first_albedo, first_normal; 
    glm::vec3 background(0); 

    //////////////////////////////////////////////////////////////////////
    // Iterate through pixels and shoot rays 

    auto start = std::chrono::high_resolution_clock::now(); 
    double progress = 0.0f;

    // #pragma omp parallel for
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            int row = j;
            int col = height - i; 

            int progress_bar_width = 70; 

            #pragma omp critical
            show_progress_bar(progress, progress_bar_width, width * height);
            progress += 1; 

            glm::vec3 pixel_color(0);

            //////////////////////////////////////////////////////////////////////

            #pragma omp parallel for
            for (int s = 0; s < samples_per_pixel; ++s){
                double u = (static_cast<double>(j) + random_double()) / width; 
                double v = (static_cast<double>(i) + random_double()) / height; 
                Ray ray = cam.get_ray(u, v); 
                glm::vec3 sample_color = shoot_ray(ray, background, world, max_depth, true, first_albedo, first_normal);
                pixel_color +=  sample_color;
            }

            //////////////////////////////////////////////////////////////////////

            auto color = std::make_shared<RGBQUAD>(write_color(pixel_color, samples_per_pixel));
            auto albedo = std::make_shared<RGBQUAD>(write_maps(first_albedo)); 
            auto normal = std::make_shared<RGBQUAD>(write_maps(first_normal)); 

            FreeImage_SetPixelColor(image, row, col, &*color);  
            FreeImage_SetPixelColor(albedo_map, row, col, &*albedo);  
            FreeImage_SetPixelColor(normal_map, row, col, &*normal);  
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