#include "../dependancies/glm/glm.hpp"
#include "ray.hpp"

namespace core {
    class Background{
    public:
        Background() {}; 
        virtual glm::vec3 get_color(const Ray& ray_in) = 0;     
    }; 

    class SolidBG : public Background{
    public:
        SolidBG(glm::vec3 bg_color) : color(bg_color) {}; 
        inline virtual glm::vec3 get_color(const Ray& ray_in) override {return color;};     
    public:
        glm::vec3 color; 
    }; 

    class GradientBG : public Background{
    public:
        GradientBG(glm::vec3 high, glm::vec3 low, float g) : high(high), low(low), gradient(g) {}; 
        virtual glm::vec3 get_color(const Ray& ray_in) override;     
    public:
        glm::vec3 high, low;
        float gradient;  
    }; 

    class ImageBG : public Background{
    public:
        ImageBG(const char* filename); 
        ImageBG() : data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

        virtual glm::vec3 get_color(const Ray& ray_in) override;     

        const static int bytes_per_pixel = 3; 
        ~ImageBG(){delete data; }
        glm::vec3 value(double u, double v, const glm::vec3& p);

    public:
        glm::vec3 high, low;
        float gradient;  

        unsigned char *data; 
        int width, height; 
        int bytes_per_scanline; 

    }; 
}