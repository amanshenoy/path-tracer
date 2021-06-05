#include "dependancies/glm/glm.hpp"
#include "ray.hpp"

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
