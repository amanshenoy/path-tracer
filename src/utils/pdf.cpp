#include "pdf.hpp"
#include "../core/objects.hpp"

namespace utils {
    CosinePDF::CosinePDF(const glm::vec3& w){ 
        uvw.build_from_w(w); 
    }

    double CosinePDF::value(glm::vec3 direction) {
        auto cosine = glm::dot(glm::normalize(direction), uvw.w());
        return (cosine <= 0) ? 0 : cosine/M_PI;
    }

    glm::vec3 CosinePDF::generate() {
        return uvw.local(sampler::random_cosine_direction());
    }

    double ObjectPDF::value(glm::vec3 direction) {
        return ptr->pdf_value(o, direction);
    }

    glm::vec3 ObjectPDF::generate() {
        return ptr->random(o);
    }

    MixturePDF::MixturePDF(std::shared_ptr<PDF> p0, std::shared_ptr<PDF> p1, float ratio) {
        p[0] = p0;
        p[1] = p1;
        li = ratio; 
    }

    double MixturePDF::value(glm::vec3 direction) {
        return li * p[0]->value(direction) + (1 - li) *p[1]->value(direction);
    }

    glm::vec3 MixturePDF::generate()   {
        if (sampler::random_double() < li)
            return p[0]->generate();
        else
            return p[1]->generate();
    }
}