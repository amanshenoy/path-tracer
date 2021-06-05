#include <algorithm>
#include <iostream>
#include <memory>

#include "utils.hpp"
#include "objects.hpp"
#include "bvh.hpp"

bool BVHNode::bounding_box(double time0, double time1, AABB& output_box){
    output_box = box;
    return true;     
}

bool BVHNode::hit(const Ray& r, double t_min, double t_max, hit_details& rec){
    if (!box.hit(r, t_min, t_max))
        return false;

    bool hit_left = left->hit(r, t_min, t_max, rec);
    bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);

    return hit_left || hit_right;
}

BVHNode::BVHNode(std::vector<std::shared_ptr<Object>>& objects, size_t start, size_t end, double time0, double time1, double& node_id, bool first) {
    int progress_bar_width = 60; 

    #pragma omp critical
    show_tree_building(node_id, progress_bar_width, static_cast<int>(objects.size()));
    int axis = 0; 
    auto comparator = (axis == 0) ? box_x_compare : (axis == 1) ? box_y_compare
                                  : box_z_compare;

    size_t object_span = end - start;

    if (object_span == 1) {
        left = right = objects[start];
        node_id += 1; 
    } 
    else if (object_span == 2) {
        node_id += 2; 
        if (comparator(objects[start], objects[start+1])){
            left = objects[start];
            right = objects[start+1];
        } 
        else {
            left = objects[start+1];
            right = objects[start];
        }
    } 
    else {
        #pragma omp parallel
        #pragma omp single
        {
            if (first) std::sort(objects.begin() + start, objects.begin() + end, comparator);
            auto mid = start + object_span/2;

            #pragma omp task
            left = std::make_shared<BVHNode>(objects, start, mid, time0, time1, node_id, false);
            
            #pragma omp task
            right = std::make_shared<BVHNode>(objects, mid, end, time0, time1, node_id, false);
        }
    }

    AABB box_left, box_right;

    if (  !left->bounding_box (time0, time1, box_left)
       || !right->bounding_box(time0, time1, box_right)
    )
        std::cerr << "No bounding box in bvh_node constructor.\n";

    box = surrounding_box(box_left, box_right);
}

bool box_compare(const std::shared_ptr<Object>& a, const std::shared_ptr<Object>& b, int axis) {
    AABB box_a;
    AABB box_b;

    if (!a->bounding_box(0,0, box_a) || !b->bounding_box(0,0, box_b))
        std::cerr << "No bounding box in bvh_node constructor.\n";

    return box_a.min()[axis] < box_b.min()[axis];
}

bool box_x_compare (const std::shared_ptr<Object>& a, const std::shared_ptr<Object>& b) {
    return box_compare(a, b, 0);
}

bool box_y_compare (const std::shared_ptr<Object>& a, const std::shared_ptr<Object>& b) {
    return box_compare(a, b, 1);
}

bool box_z_compare (const std::shared_ptr<Object>& a, const std::shared_ptr<Object>& b) {
    return box_compare(a, b, 2);
}