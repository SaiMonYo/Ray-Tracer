#pragma once

#include "AABB.h"
#include <iostream>
#include <vector>
#include <set>

class SpaceTreeNode{
    public:
        SpaceTreeNode(){
        }

        SpaceTreeNode(Vector3 min_, Vector3 max_){
            center = (max_ + min_) * 0.5f;
            extents = max_ - center;
            box[0] = min_;
            box[1] = max_;
        }

        ~SpaceTreeNode(){
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
            SpaceTreeNode TRF = SpaceTreeNode(center, v1);
            TRF.build(depth - 1);
            SpaceTreeNode TLF = SpaceTreeNode(Vector3(v0.x, center.y, center.z), Vector3(center.x, v1.y, v1.z));
            TLF.build(depth - 1);
            SpaceTreeNode BLF = SpaceTreeNode(Vector3(v0.x, v0.y, center.z), Vector3(center.x, center.y, v1.z));
            BLF.build(depth - 1);
            SpaceTreeNode BRF = SpaceTreeNode(Vector3(center.x, v0.y, center.z), Vector3(v1.x, center.y, v1.z));
            BRF.build(depth - 1);

            SpaceTreeNode BRB = SpaceTreeNode(Vector3(center.x, v0.y, v0.z), Vector3(v1.x, center.y, center.z));
            BRB.build(depth - 1);
            SpaceTreeNode BLB = SpaceTreeNode(v0, center);
            BLB.build(depth - 1);
            SpaceTreeNode TLB = SpaceTreeNode(Vector3(v0.x, center.y, v0.z), Vector3(center.x, v1.y, center.z));
            TLB.build(depth - 1);
            SpaceTreeNode TRB = SpaceTreeNode(Vector3(center.x, center.y, v0.z), Vector3(v1.x, v1.y, center.z));
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
                return AABBIntersection(center, extents, tri);
                
            }
            // within in the current box
            if (AABBIntersection(center, extents, tri)){
                for (SpaceTreeNode& child: children){
                    if (child.insert(tri, depth -1)){
                        child.faces.push_back(tri);
                    }
                }
            }
            return false;
        }

        bool intersection(Ray ray, RayHit& inter){
            bool hit = false;
            float t = AABBIntersection(box[0], box[1], ray);
            if (t > 0 && AABBIntersection(box[0], box[1], ray) < inter.distance){
                // at bottom if no children
                if (children.size() == 0){
                    // intersect triangles
                    for (auto& tri: faces){
                        hit |= ray_triangle(ray, inter, tri);
                    }
                    return hit;
                }
                // check children
                for (SpaceTreeNode& node: children){
                    hit |= node.intersection(ray, inter);
                }
            }
            return hit;
        }

        bool cull(){
            std::vector<SpaceTreeNode> tokeep;
            for (SpaceTreeNode& node: children){
                if (!node.cull()){
                    tokeep.push_back(node);
                }
            }
            children.clear();
            children = tokeep;
            return (children.size() == 0 && faces.size() == 0);
        }

    public:
        Vector3 box[2];
        Vector3 center;
        Vector3 extents;
        std::vector<SpaceTreeNode> children;
        std::vector<Triangle> faces;
};

class Octree{
    public:
        Octree(){
        }

        Octree(Vector3 boundingBox_[2], std::vector<std::vector<int> >& faces_, std::vector<Vector3>& vertices_, uint8_t depth_){
            // add the meshes bounding box to the roots 
            root = SpaceTreeNode(boundingBox_[0], boundingBox_[1]);
            depth = depth_;
            vertices = vertices_;
            root.build(depth);
            for (int i = 0; i < faces_.size(); i++){
                // inserting each face into the tree
                std::vector<int> face = faces_[i];
                for (SpaceTreeNode& child: root.children){
                    Triangle tri = Triangle(vertices[face[0] - 1], vertices[face[3] - 1], vertices[face[6] - 1], i);
                    if (child.insert(tri, depth)){
                        child.faces.push_back(tri);
                    }
                }
            }
            root.cull();
        }

        ~Octree(){
            root = SpaceTreeNode();
            vertices.clear();
        }

        bool intersection(Ray ray, RayHit& inter){
            // doesn't intersect with root nodes bounding box
            if (AABBIntersection(root.box[0], root.box[1], ray) < 0){
                // return empty array
                return false;
            }
            // find the faces we need to test
            bool hit = false;
            for (SpaceTreeNode& node: root.children){
                hit |= node.intersection(ray, inter);
            }
            return hit;
        }

    public:
        uint8_t depth;
        Vector3 box[2];
        std::vector<Vector3> vertices;
        SpaceTreeNode root;
};