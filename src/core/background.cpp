#include <iostream>

#include "../utils/utils.hpp"
#include "background.hpp"
#include "../dependancies/colors.hpp"
#include "../dependancies/stb_image.h"

namespace core {
    glm::vec3 GradientBG::get_color(const Ray& ray_in){
        glm::vec3 unit_direction = abs(glm::normalize(ray_in.direction()));
        float t = gradient * unit_direction.y;
        return (1 - t) * low + t * high;
    }

    ImageBG::ImageBG(const char* filename) {
        int components_per_pixel = bytes_per_pixel;
        data = stbi_load(filename, &width, &height, &components_per_pixel, components_per_pixel);
        if (!data){
            std::cerr << BOLDCYAN << "[ ERROR ]" << RESET << " Could not load image file " << filename << '\n';
        }
        bytes_per_scanline = bytes_per_pixel * width; 
    }

    glm::vec3 ImageBG::get_color(const Ray& ray_in){
        glm::vec3 normalised_direction = glm::normalize(ray_in.direction());
        auto theta = acos(-normalised_direction.y);
        auto phi = atan2(-normalised_direction.z, normalised_direction.x) + M_PI;

        auto u = phi / (2*M_PI);
        auto v = theta / M_PI;
        return value(u, v, normalised_direction); 
    }

    glm::vec3 ImageBG::value(double u, double v, const glm::vec3& p) {
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
}