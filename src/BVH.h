#pragma once

#include "SpaceTree.h"

struct BVHNode{
    Vector3 corners[2];
    uint left_child, first_index, tri_count;
    bool is_leaf(){return tri_count > 0;}

    BVHNode(){
        corners[0] = Vector3(1e8f);
        corners[1] = Vector3(-1e8f);
        left_child = -1;
        first_index = -1;
        tri_count = 0;
    }
};



struct BVH: public SpaceTree{
    std::vector<BVHNode> nodes;
    std::vector<Triangle> triangles;
    std::vector<int> indices;

    uint root_index;
    uint nodes_used;
    uint N;

    BVH(){
        root_index = 0;
        nodes_used = 1;
    }

    BVH(std::vector<Triangle>& tris){
        triangles = tris;
        N = triangles.size();
        indices.resize(N);
        for (int i = 0; i < N; i++){
            indices[i] = i;
        }
        nodes.resize(N * 2 - 1);
        for (int i = 0; i < N; i++){
            Vector3 v0 = triangles[i].vertices[0];
            Vector3 v1 = triangles[i].vertices[1];
            Vector3 v2 = triangles[i].vertices[2];
        }
        root_index = 0;
        nodes_used = 1;
        build();
    }

    void build(){
        BVHNode& root = nodes[root_index];
        root.first_index = 0;
        root.tri_count = N;
        refit_node(root_index);
        divide(root_index);
    }


    void refit_node(uint ind){
        BVHNode& node = nodes[ind];
        node.corners[0] = Vector3(1e8f);
        node.corners[1] = Vector3(-1e8f);
        uint first = node.first_index;
        for (uint i = 0; i < node.tri_count; i++){
            Triangle& tri = triangles[indices[first + i]];
            for (int j = 0; j < 3; j++){
                node.corners[0] = Vector3::min(node.corners[0], tri.vertices[j]);
                node.corners[1] = Vector3::max(node.corners[1], tri.vertices[j]);
            }
        }
    }

    void divide(uint ind){
        BVHNode& node = nodes[ind];
        if (node.tri_count <= 2){
            return;
        }
        Vector3 extents = node.corners[1] - node.corners[0];
        uint axis = 0;
        if (extents.y > extents.x) axis = 1;
        if (extents.z > extents[axis]) axis = 2;

        float split = node.corners[0][axis] + extents[axis] * 0.5f;
        uint i = node.first_index;
        uint j = i + node.tri_count - 1;

        while (i <= j){
            if (triangles[indices[i]].centroid[axis] < split){
                i++;
            }
            else{
                std::swap(indices[i], indices[j]);
                j--;
            }
        }

        uint left_count = i - node.first_index;
        if (left_count == 0 || left_count == node.tri_count){
            return;
        }
        uint left_ind = nodes_used++;
        uint right_ind = nodes_used++;
        nodes[left_ind].first_index = node.first_index;
        nodes[left_ind].tri_count = left_count;
        nodes[right_ind].first_index = i;
        nodes[right_ind].tri_count = node.tri_count - left_count;
        node.left_child = left_ind;
        node.tri_count = 0;
        refit_node(left_ind);
        refit_node(right_ind);

        divide(left_ind);
        divide(right_ind);
    }

    bool intersect_BVH(const Ray& ray, RayHit& inter, uint ind){
        bool hit = false;
        BVHNode& node = nodes[ind];
        float t = AABBIntersection(node.corners[0], node.corners[1], ray);
        if (t < 0 || t > inter.distance){
            return false;
        }
        if (node.is_leaf()){
            for (uint i = 0; i < node.tri_count; i++){
                Triangle& tri = triangles[indices[node.first_index + i]];
                hit |= ray_triangle(ray, inter, tri);
            }
        }
        else{
            hit |= intersect_BVH(ray, inter, node.left_child);
            hit |= intersect_BVH(ray, inter, node.left_child + 1);
        }
        return hit;
    }

    bool intersection(const Ray& ray, RayHit& inter){
        bool hit = false;
        float t = AABBIntersection(nodes[root_index].corners[0], nodes[root_index].corners[1], ray);
        if (t > 0 && t < inter.distance){
            hit = intersect_BVH(ray, inter, root_index);
        }
        return hit;
    }
};