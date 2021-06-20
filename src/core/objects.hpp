#include <memory>
#include <vector>

#include "materials.hpp"
#include "../optim/aabb.hpp"

namespace core {
    class Object{
    public:
        virtual bool hit(const Ray& ray, double t_min, double t_max, utils::hit_details& rec) = 0;
        virtual bool bounding_box(double time0, double time1, optim::AABB& output_box) = 0;
        virtual double pdf_value( glm::vec3& o,  glm::vec3& v) {
            return 0.0;
        }

        virtual glm::vec3 random(glm::vec3& o) {
            return glm::vec3(1, 0, 0);
        }
    };

    class Sphere: public Object{
    public:
        Sphere() {};
        Sphere(glm::vec3 center_t1, glm::vec3 center_t2, double start, double end,\
            double radius, std::shared_ptr<Material> material) 
            : center_t1(center_t1), center_t2(center_t2), t1(start), t2(end), \
                    radius(radius), mat_ptr(material) {};
        Sphere(glm::vec3 center, double radius, std::shared_ptr<Material> material) 
            : center_t1(center), center_t2(center), t1(0), t2(1), \
                    radius(radius), mat_ptr(material) {};


        virtual bool hit(const Ray& ray, double t_min, double t_max, utils::hit_details& rec) override;
        virtual bool bounding_box(double time0, double time1, optim::AABB& output_box) override;
        virtual double pdf_value( glm::vec3& o,  glm::vec3& v);
        virtual glm::vec3 random(glm::vec3& o);
        static void get_sphere_uv(const glm::vec3& p, double& u, double& v);
        glm::vec3 center(double time) const; 

    public:
        glm::vec3 center_t1, center_t2;
        double t1, t2; 
        double radius; 
        std::shared_ptr<Material> mat_ptr;
    };

    class RectangleXY : public Object{
    public:
        RectangleXY() {}
        RectangleXY(double x1, double y1, double x2, double y2, double k, \
            std::shared_ptr<Material> mat) : x1(x1), y1(y1), x2(x2), y2(y2), k(k), mat(mat) {}

        virtual bool hit(const Ray& ray, double t_min, double t_max, utils::hit_details& rec) override;
        virtual bool bounding_box(double time0, double time1, optim::AABB& output_box) override;

    public:
        double x1, x2, y1, y2; 
        double k; 
        std::shared_ptr<Material> mat; 
    };

    class RectangleYZ : public Object{
    public:
        RectangleYZ() {}
        RectangleYZ(double y1, double z1, double y2, double z2, double k, \
            std::shared_ptr<Material> mat) : y1(y1), z1(z1), y2(y2), z2(z2), k(k), mat(mat) {}

        virtual bool hit(const Ray& ray, double t_min, double t_max, utils::hit_details& rec) override;
        virtual bool bounding_box(double time0, double time1, optim::AABB& output_box) override;

    public:
        double y1, y2, z1, z2; 
        double k; 
        std::shared_ptr<Material> mat; 
    };

    class RectangleXZ : public Object{
    public:
        RectangleXZ() {}
        RectangleXZ(double x1, double z1, double x2, double z2, double k, \
            std::shared_ptr<Material> mat) : x1(x1), z1(z1), x2(x2), z2(z2), k(k), mat(mat) {}

        virtual bool hit(const Ray& ray, double t_min, double t_max, utils::hit_details& rec) override;
        virtual bool bounding_box(double time0, double time1, optim::AABB& output_box) override;
        
        virtual double pdf_value(glm::vec3& origin, glm::vec3& v)  override {
            utils::hit_details rec;
            double infinity = std::numeric_limits<double>::infinity();
            if (!this->hit(Ray(origin, v, 0.5), 0.001, infinity, rec))
                return 0;

            auto area = (x2-x1)*(z2-z1);
            auto distance_squared = rec.t * rec.t * pow(glm::length(v), 2);
            auto cosine = fabs(dot(v, rec.normal) / v.length());

            return distance_squared / (cosine * area);
        }

        virtual glm::vec3 random(glm::vec3& o) override {
            auto random_point = glm::vec3(utils::sampler::random_double(x1, x2), k, utils::sampler::random_double(z1, z2));
            return glm::normalize(random_point - o);
        }

    public:
        double x1, x2, z1, z2; 
        double k; 
        std::shared_ptr<Material> mat; 
    };

    class Scene : public Object{
    public:
        Scene() {}
        Scene(std::shared_ptr<Object> object){add(object);}

        void clear() {objects.clear();}
        void add(std::shared_ptr<Object> object){ objects.push_back(object);}

        virtual bool hit(const Ray& ray, double t_min, double t_max, utils::hit_details& rec) override;
        virtual bool bounding_box(double time0, double time1, optim::AABB& output_box) override;
        virtual double pdf_value( glm::vec3& o,  glm::vec3& v) override;
        virtual glm::vec3 random(glm::vec3& o) override;

    public:
        std::vector<std::shared_ptr<Object>> objects; 
    };

    class Box : public Object{
    public:
        Box(){}
        Box(const glm::vec3& p1, const glm::vec3& p2, std::shared_ptr<Material> ptr);

        virtual bool hit(const Ray& ray, double t_min, double t_max, utils::hit_details& rec) override;
        virtual bool bounding_box(double time0, double time1, optim::AABB& output_box) override;
    public:
        glm::vec3 box_min, box_max;
        Scene faces;
    };

    class Translate : public Object{
    public:
        Translate(std::shared_ptr<core::Object> obj, const glm::vec3 displacement)
            : ptr(obj), translation(displacement) {}
        
        virtual bool hit(const Ray& ray, double t_min, double t_max, utils::hit_details& rec) override;
        virtual bool bounding_box(double time0, double time1, optim::AABB& output_box) override;

    public:
        std::shared_ptr<Object> ptr; 
        glm::vec3 translation; 
    };

    class RotateY : public Object {
    public:
        RotateY(std::shared_ptr<core::Object> p, double angle);

        virtual bool hit(const Ray& ray, double t_min, double t_max, utils::hit_details& rec) override;
        virtual bool bounding_box(double time0, double time1, optim::AABB& output_box) override;

        public:
            std::shared_ptr<Object> ptr;
            double sin_theta;
            double cos_theta;
            bool hasbox;
            optim::AABB bbox;
    };

    class ConstantMedium : public Object{
    public:
        ConstantMedium(std::shared_ptr<Object> b, double d, std::shared_ptr<Texture> a)
            : boundary(b), neg_inv_dens(-1/d), phase_fn(std::make_shared<Isotropic>(a)) {}
        ConstantMedium(std::shared_ptr<Object> b, double d, glm::vec3 c)
            : boundary(b), neg_inv_dens(-1/d), phase_fn(std::make_shared<Isotropic>(c)) {}

        virtual bool hit(const Ray& ray, double t_min, double t_max, utils::hit_details& rec) override;
        virtual bool bounding_box(double time0, double time1, optim::AABB& output_box) override;

    public:
        std::shared_ptr<Object> boundary; 
        std::shared_ptr<Material> phase_fn;
        double neg_inv_dens; 
    };

    class Triangle: public Object{
    public:
        Triangle() {};
        Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, std::shared_ptr<Material> mat)
            : v_0(v0), v_1(v1), v_2(v2), normal(glm::normalize(glm::cross(v1 - v0, v2 - v0))), mat(mat) {}
        Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 n, std::shared_ptr<Material> mat)
            : v_0(v0), v_1(v1), v_2(v2), normal(n), mat(mat) {}

        virtual bool hit(const Ray& ray, double t_min, double t_max, utils::hit_details& rec) override;
        virtual bool bounding_box(double time0, double time1, optim::AABB& output_box) override;

    public: 
        glm::vec3 v_0, v_1, v_2; 
        glm::vec3 normal; 
        std::shared_ptr<Material> mat; 
    };

    class Mesh: public Object{
    public:
        Mesh() {};
        Mesh(std::string obj_path, std::shared_ptr<Material> material, bool evaluate_normals);
        Mesh(std::string obj_path, std::shared_ptr<Material> material);
        void parser(bool evaluate_normals);
        Scene get_mesh(); 

        virtual bool hit(const Ray& ray, double t_min, double t_max, utils::hit_details& rec) override;
        virtual bool bounding_box(double time0, double time1, optim::AABB& output_box) override;

    public:
        float minX = INFINITY, minY = INFINITY, minZ = INFINITY;
        float maxX = -INFINITY, maxY = -INFINITY, maxZ = -INFINITY;    
        std::string path;
        std::vector<glm::vec3> vertices, normals; 
        std::vector<unsigned int> indices; 
        Scene mesh;
        std::shared_ptr<Material> mat; 
    }; 

    class Flip : public Object {
        public:
            Flip(std::shared_ptr<Object> p) : ptr(p) {}
            virtual bool hit(const Ray& r, double t_min, double t_max, utils::hit_details& rec) override {
                if (!ptr->hit(r, t_min, t_max, rec))
                    return false;

                rec.front_face = !rec.front_face;
                return true;
            }

            virtual bool bounding_box(double time0, double time1, optim::AABB& output_box) override {
                return ptr->bounding_box(time0, time1, output_box);
            }

        public:
            std::shared_ptr<Object> ptr;
    };
}