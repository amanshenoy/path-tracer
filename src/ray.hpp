#pragma once
#include "dependancies/glm/glm.hpp"

class Ray{
public: 
    Ray(){}
    Ray(glm::vec3 origin, glm::vec3 direction, double time) : o(origin), d(direction), t(time) {}
    
    glm::vec3 origin() const;
    glm::vec3 direction() const;
    double time() const;

    glm::vec3 at(double t) const;

private:
    glm::vec3 o, d;
    double t; 
};