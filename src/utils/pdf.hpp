#pragma once 
#include <memory>
#include "utils.hpp"

namespace core {class Object;} 

namespace utils { 
    class PDF {
        public:
            virtual ~PDF() {}
            virtual double value(glm::vec3 direction) = 0;
            virtual glm::vec3 generate() = 0;
    };

    class CosinePDF : public PDF {
        public:
            CosinePDF(const glm::vec3& w);
            virtual double value(glm::vec3 direction) override;
            virtual glm::vec3 generate() override;

        public:
            LocalBasis uvw;
    };

    class ObjectPDF : public PDF {
        public:
            ObjectPDF(std::shared_ptr<core::Object> p, const glm::vec3& origin) : ptr(p), o(origin) {}
            virtual double value(glm::vec3 direction) override;
            virtual glm::vec3 generate() override;

        public:
            glm::vec3 o;
            std::shared_ptr<core::Object> ptr;
    };

    class MixturePDF : public PDF {
        public:
            MixturePDF(std::shared_ptr<PDF> p0, std::shared_ptr<PDF> p1, float ratio);
            virtual double value(glm::vec3 direction) override;
            virtual glm::vec3 generate()  override ;

        public:
            std::shared_ptr<PDF> p[2];
            float li; 
    };
}