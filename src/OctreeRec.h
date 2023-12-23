#pragma once

#include "AABB.h"
#include "Triangle.h"
#include <iostream>
#include <vector>
#include <set>

struct OctreeNode{
    AABB box;
    Vector3 center;
    Vector3 extents;
    std::vector<OctreeNode> children;
    std::vector<Triangle> faces;

    OctreeNode(){
    }

    OctreeNode(Vector3 min_, Vector3 max_){
        center = (max_ + min_) * 0.5f;
        extents = max_ - center;
        box[0] = min_;
        box[1] = max_;
    }

    ~OctreeNode(){
        children.clear();
        faces.clear();
    }

    void build(int depth){
        if (depth == 0){
            return;
        }
        Vector3 v0 = box[0];
        Vector3 v1 = box[1];
        // TOP/BOTTOM LEFT/RIGHT FRONT/BACK
        OctreeNode TRF = OctreeNode(center, v1);
        TRF.build(depth - 1);
        OctreeNode TLF = OctreeNode(Vector3(v0.x, center.y, center.z), Vector3(center.x, v1.y, v1.z));
        TLF.build(depth - 1);
        OctreeNode BLF = OctreeNode(Vector3(v0.x, v0.y, center.z), Vector3(center.x, center.y, v1.z));
        BLF.build(depth - 1);
        OctreeNode BRF = OctreeNode(Vector3(center.x, v0.y, center.z), Vector3(v1.x, center.y, v1.z));
        BRF.build(depth - 1);

        OctreeNode BRB = OctreeNode(Vector3(center.x, v0.y, v0.z), Vector3(v1.x, center.y, center.z));
        BRB.build(depth - 1);
        OctreeNode BLB = OctreeNode(v0, center);
        BLB.build(depth - 1);
        OctreeNode TLB = OctreeNode(Vector3(v0.x, center.y, v0.z), Vector3(center.x, v1.y, center.z));
        TLB.build(depth - 1);
        OctreeNode TRB = OctreeNode(Vector3(center.x, center.y, v0.z), Vector3(v1.x, v1.y, center.z));
        TRB.build(depth - 1);

        children.push_back(TRF);
        children.push_back(TLF);
        children.push_back(BLF);
        children.push_back(BRF);
        children.push_back(BRB);
        children.push_back(BLB);
        children.push_back(TLB);
        children.push_back(TRB);
    }

    bool insert(Triangle tri, uint8_t depth){
        if (depth <= 1){
            // within the insertion box
            if (AABBIntersection(center, extents, tri)){
                faces.push_back(tri);
                return true;
            }
            
        }
        // within in the current box
        if (AABBIntersection(center, extents, tri)){
            for (OctreeNode& child: children){
                child.insert(tri, depth -1);
            }
        }
        return false;
    }

    bool intersection(Ray ray, RayHit& inter){
        bool hit = false;
        float t = AABBIntersection(box, ray);
        if (t != FINF && AABBIntersection(box, ray) < inter.distance){
            // at bottom if no children
            if (children.size() == 0){
                // intersect triangles
                for (auto& tri: faces){
                    hit |= tri.intersect(ray, inter);
                }
                return hit;
            }
            // check children
            for (OctreeNode& node: children){
                hit |= node.intersection(ray, inter);
            }
        }
        return hit;
    }

    bool cull(){
        std::vector<OctreeNode> tokeep;
        for (OctreeNode& node: children){
            if (!node.cull()){
                tokeep.push_back(node);
            }
        }
        children.clear();
        children = tokeep;
        return (children.size() == 0 && faces.size() == 0);
    }
};

struct Octree: public Observable{
    uint8_t depth;
    Vector3 box[2];
    std::vector<Vector3> vertices;
    OctreeNode root;

    Octree(){
    }

    Octree(Vector3 boundingBox_[2], std::vector<std::vector<int> >& faces_, std::vector<Vector3>& vertices_, uint8_t depth_){
        // add the meshes bounding box to the roots 
        root = OctreeNode(boundingBox_[0], boundingBox_[1]);
        depth = depth_;
        vertices = vertices_;
        root.build(depth);
        for (int i = 0; i < faces_.size(); i++){
            // inserting each face into the tree
            std::vector<int> face = faces_[i];
            for (OctreeNode& child: root.children){
                Triangle tri = Triangle(vertices[face[0] - 1], vertices[face[3] - 1], vertices[face[6] - 1], i);
                child.insert(tri, depth);
            }
        }
        root.cull();
    }

    ~Octree(){
        root = OctreeNode();
        vertices.clear();
    }

    bool intersection(const Ray& ray, RayHit& inter){
        // doesn't intersect with root nodes bounding box
        if (AABBIntersection(root.box, ray) == FINF){
            // return empty array
            return false;
        }
        // find the faces we need to test
        bool hit = false;
        for (OctreeNode& node: root.children){
            hit |= node.intersection(ray, inter);
        }
        return hit;
    }
};