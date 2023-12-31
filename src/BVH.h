#pragma once

#include "Observable.h"
#include "AABB.h"
#include "Triangle.h"

#define REC_INTERSECTION 0

struct BVHNode{
    AABB aabb;
    uint left_child, first_index, observable_count;
    bool is_leaf(){return observable_count > 0;}

    BVHNode(){
        aabb = AABB();
        left_child = -1;
        first_index = -1;
        observable_count = 0;
    }
};


struct BVHSplitBucket{
    uint count = 0;
    AABB box;
};


struct BVH: public Observable{
    std::vector<BVHNode> nodes;
    std::vector<std::shared_ptr<Observable>> observables;
    std::vector<int> indices;

    uint root_index;
    uint nodes_used;
    uint N;

    Vector3 min_vertex(){
        return nodes[root_index].aabb.min;
    }

    Vector3 max_vertex(){
        return nodes[root_index].aabb.max;
    }

    Vector3 centroid(){
        return nodes[root_index].aabb.center();
    }

    BVH(){
        root_index = 0;
        nodes_used = 1;
    }

    BVH(std::vector<std::shared_ptr<Observable>>& obs){
        observables = obs;
        N = observables.size();
        indices.resize(N);
        for (int i = 0; i < N; i++){
            indices[i] = i;
        }
        nodes.resize(N * 2 - 1);
        root_index = 0;
        nodes_used = 1;
        build();
    }

    void build(){
        BVHNode& root = nodes[root_index];
        root.first_index = 0;
        root.observable_count = N;
        refit_node(root_index);
        sah_divide(root_index);
    }


    void refit_node(uint ind){
        BVHNode& node = nodes[ind];
        AABB& box = node.aabb;
        box.min = Vector3(1e8f);
        box.max = Vector3(-1e8f);
        uint first = node.first_index;
        for (uint i = 0; i < node.observable_count; i++){
            std::shared_ptr<Observable> obs = observables[indices[first + i]];
            box.fix(obs);
        }
    }

    void basic_divide(uint ind){
        BVHNode& node = nodes[ind];
        if (node.observable_count <= 2){
            return;
        }
        AABB& box = node.aabb;
        Vector3 extents = box.extents();
        uint axis = 0;
        if (extents.y > extents.x) axis = 1;
        if (extents.z > extents[axis]) axis = 2;

        float split = box.min[axis] + extents[axis] * 0.5f;
        uint i = node.first_index;
        uint j = i + node.observable_count - 1;

        while (i <= j){
            if (observables[indices[i]]->centroid()[axis] < split){
                i++;
            }
            else{
                std::swap(indices[i], indices[j]);
                j--;
            }
        }

        uint left_count = i - node.first_index;
        if (left_count == 0 || left_count == node.observable_count){
            return;
        }
        uint left_ind = nodes_used++;
        uint right_ind = nodes_used++;
        nodes[left_ind].first_index = node.first_index;
        nodes[left_ind].observable_count = left_count;
        nodes[right_ind].first_index = i;
        nodes[right_ind].observable_count = node.observable_count - left_count;
        node.left_child = left_ind;
        node.observable_count = 0;
        refit_node(left_ind);
        refit_node(right_ind);

        basic_divide(left_ind);
        basic_divide(right_ind);
    }

    void sah_divide(uint ind){
        BVHNode& node = nodes[ind];
        if (node.observable_count <= 2){
            return;
        }
        constexpr int nbuckets = 20;
        BVHSplitBucket buckets[nbuckets];

        AABB& nodes_box = node.aabb;
        Vector3 extents = nodes_box.extents();
        uint axis = 0;
        if (extents.y > extents.x) axis = 1;
        if (extents.z > extents[axis]) axis = 2;

        for (uint i = 0; i < node.observable_count; i++){
            int b = nbuckets * nodes_box.offset(observables[indices[node.first_index + i]]->centroid())[axis];
            if (b == nbuckets) b = nbuckets - 1;
            buckets[b].count++;
            buckets[b].box.fix(observables[indices[node.first_index + i]]);
        }

        constexpr int nsplits = nbuckets - 1;
        float cost[nsplits];
        for (int i = 0; i < nsplits; i++){
            AABB box0, box1;
            uint count0 = 0, count1 = 0;
            for (int j = 0; j <= i; j++){
                box0.fix(buckets[j].box);
                count0 += buckets[j].count;
            }
            for (int j = i + 1; j < nbuckets; j++){
                box1.fix(buckets[j].box);
                count1 += buckets[j].count;
            }
            cost[i] = 0.125f + (count0 * box0.area() + count1 * box1.area()) / nodes_box.area();
        }
        float min_cost = cost[0];
        int min_cost_split = 0;
        for (int i = 1; i < nsplits; i++){
            if (cost[i] < min_cost){
                min_cost = cost[i];
                min_cost_split = i;
            }
        }

        float leaf_cost = node.observable_count;
        if (node.observable_count > 2 || min_cost < leaf_cost){
            float split = nodes_box.min[axis] + extents[axis] * (min_cost_split + 1) / nbuckets;

            // cant use uints here as j may dip below 0
            int i = node.first_index;
            int j = i + node.observable_count - 1;

            while (i <= j){
                if (observables[indices[i]]->centroid()[axis] < split){
                    i++;
                }
                else{
                    std::swap(indices[i], indices[j]);
                    j--;
                }
            }

            uint left_count = i - node.first_index;
            if (left_count == 0 || left_count == node.observable_count){
                return;
            }
            uint left_ind = nodes_used++;
            uint right_ind = nodes_used++;
            nodes[left_ind].first_index = node.first_index;
            nodes[left_ind].observable_count = left_count;
            nodes[right_ind].first_index = i;
            nodes[right_ind].observable_count = node.observable_count - left_count;
            node.left_child = left_ind;
            node.observable_count = 0;
            refit_node(left_ind);
            refit_node(right_ind);

            sah_divide(left_ind);
            sah_divide(right_ind);
        }
    }

#if REC_INTERSECTION 
    bool intersect_BVH(const Ray& ray, RayHit& inter, uint ind){
        bool hit = false;
        BVHNode& node = nodes[ind];
        float t = AABBIntersection(node.aabb, ray);
        if (t < 0 || t > inter.distance){
            return false;
        }
        if (node.is_leaf()){
            for (uint i = 0; i < node.observable_count; i++){
                Triangle& tri = observables[indices[node.first_index + i]];
                hit |= ray_triangle(ray, inter, tri);
            }
        }
        else{
            hit |= intersect_BVH(ray, inter, node.left_child);
            hit |= intersect_BVH(ray, inter, node.left_child + 1);
        }
        return hit;
    }

    bool intersect(const Ray& ray, RayHit& inter){
        bool hit = false;
        float t = AABBIntersection(nodes[root_index].aabb, ray);
        if (t > 0 && t < inter.distance){
            hit = intersect_BVH(ray, inter, root_index);
        }
        return hit;
    }
#else
    bool intersect(const Ray& ray, RayHit& inter){
        if (AABBIntersection(nodes[root_index].aabb, ray) == FINF){
            return false;
        }
        bool hit = false;
        BVHNode* node = &nodes[root_index], *stack[200];
        uint stack_ptr = 0;
        while (true){
            if (node->is_leaf()){
                uint first = node->first_index;
                for (uint i = 0; i < node->observable_count; i++){
                    std::shared_ptr<Observable>& obs = observables[indices[first + i]];
                    hit |= obs->intersect(ray, inter);
                }
                if (stack_ptr == 0){
                    break;
                }
                else{
                    node = stack[--stack_ptr];
                }
                continue;
            }
            BVHNode* child1 = &nodes[node->left_child];
            BVHNode* child2 = &nodes[node->left_child + 1];
            float t1 = AABBIntersection(child1->aabb, ray);
            float t2 = AABBIntersection(child2->aabb, ray);
            if (t1 > t2){
                std::swap(child1, child2);
                std::swap(t1, t2);
            }
            if (t1 == FINF || t1 > inter.distance){
                if (stack_ptr == 0){
                    break;
                }
                else{
                    node = stack[--stack_ptr];
                }
            }
            else{
                node = child1;
                if (t2 != FINF && t2 < inter.distance){
                    stack[stack_ptr++] = child2;
                }
            }
        }
        return hit;
    }
#endif
};