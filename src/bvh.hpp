#pragma once
#include <memory>
#include <vector>

#include "dependancies/glm/glm.hpp"
#include "ray.hpp"

class BVHNode : public Object {
    public:
        BVHNode();

        BVHNode(const std::shared_ptr<Mesh>& mesh, double time0, double time1);

        BVHNode(Scene& list, double time0, double time1)
            : BVHNode(list.objects, 0, list.objects.size(), time0, time1, num) {}

        BVHNode(std::vector<std::shared_ptr<Object>>& src_objects,
            size_t start, size_t end, double time0, double time1, double node_id);

        virtual bool hit(const Ray& r, double t_min, double t_max, hit_details& rec) override;
        virtual bool bounding_box(double time0, double time1, AABB& output_box) override;

    public:
        std::shared_ptr<Object> left;
        std::shared_ptr<Object> right;
        AABB box;
        double num = 0; 
};

bool box_compare(const std::shared_ptr<Object>& a, const std::shared_ptr<Object>& b, int axis);

bool box_x_compare (const std::shared_ptr<Object>& a, const std::shared_ptr<Object>& b);

bool box_y_compare (const std::shared_ptr<Object>& a, const std::shared_ptr<Object>& b);

bool box_z_compare (const std::shared_ptr<Object>& a, const std::shared_ptr<Object>& b);