#pragma once

#include "AABB.h"
#include <iostream>
#include <vector>
#include <stack>
#include <set>

struct SpaceTreeNode{
    Vector3 box[2];
    Vector3 centre;
    Vector3 extents;
    int index = -1;
    bool is_leaf = false;
    bool is_dead = false;
    std::vector<std::vector<int>> faces;

    SpaceTreeNode(){
    }

    SpaceTreeNode(Vector3 min_, Vector3 max_, int ind){
        centre = (max_ + min_) * 0.5f;
        extents = max_ - centre;
        box[0] = min_;
        box[1] = max_;
        index = ind;
    }

    ~SpaceTreeNode(){
        faces.clear();
    }
};

struct Octree{
    uint8_t depth;
    SpaceTreeNode root;
    std::vector<Vector3> vertices;
    std::vector<SpaceTreeNode> nodes;

    Octree(){
    }

    Octree(Vector3 boundingBox_[2], std::vector<std::vector<int>>& faces_, std::vector<Vector3>& vertices_, uint8_t depth_){
        // add the meshes bounding box to the roots 
        root = SpaceTreeNode(boundingBox_[0], boundingBox_[1], -1);
        depth = depth_;
        vertices = vertices_;
        build();
        insert(faces_);
    }

    ~Octree(){
        vertices.clear();
        nodes.clear();
    }

    void build(){
        uint32_t size = 8 * (pow(8, depth) - 1) / 7;
        nodes.resize(size);
        std::stack<SpaceTreeNode> stack;
        stack.push(root);

        while (!stack.empty()){
            SpaceTreeNode node = stack.top();
            stack.pop();
            int i = node.index;
            Vector3 v0 = node.box[0];
            Vector3 v1 = node.box[1];
            Vector3 centre = node.centre;

            uint32_t base_index = (i+1) * 8;
            SpaceTreeNode TRF = SpaceTreeNode(centre, v1, base_index);
            SpaceTreeNode TLF = SpaceTreeNode(Vector3(v0.x, centre.y, centre.z), Vector3(centre.x, v1.y, v1.z), base_index + 1);
            SpaceTreeNode BLF = SpaceTreeNode(Vector3(v0.x, v0.y, centre.z), Vector3(centre.x, centre.y, v1.z), base_index + 2);
            SpaceTreeNode BRF = SpaceTreeNode(Vector3(centre.x, v0.y, centre.z), Vector3(v1.x, centre.y, v1.z), base_index + 3);

            SpaceTreeNode BRB = SpaceTreeNode(Vector3(centre.x, v0.y, v0.z), Vector3(v1.x, centre.y, centre.z), base_index + 4);
            SpaceTreeNode BLB = SpaceTreeNode(v0, centre, base_index + 5);
            SpaceTreeNode TLB = SpaceTreeNode(Vector3(v0.x, centre.y, v0.z), Vector3(centre.x, v1.y, centre.z), base_index + 6);
            SpaceTreeNode TRB = SpaceTreeNode(Vector3(centre.x, centre.y, v0.z), Vector3(v1.x, v1.y, centre.z), base_index + 7);


            if ((base_index+1) * 8 < size){
                stack.push(TRF);
                stack.push(TLF);
                stack.push(BLF);
                stack.push(BRF);
                stack.push(BRB);
                stack.push(BLB);
                stack.push(TLB);
                stack.push(TRB);
            }
            else{
                TRF.is_leaf = true;
                TLF.is_leaf = true;
                BLF.is_leaf = true;
                BRF.is_leaf = true;
                BRB.is_leaf = true;
                BLB.is_leaf = true;
                TLB.is_leaf = true;
                TRB.is_leaf = true;
            }
            nodes[base_index    ] = TRF;
            nodes[base_index + 1] = TLF;
            nodes[base_index + 2] = BLF;
            nodes[base_index + 3] = BRF;
            nodes[base_index + 4] = BRB;
            nodes[base_index + 5] = BLB;
            nodes[base_index + 6] = TLB;
            nodes[base_index + 7] = TRB;
        }
    }

    void insert(std::vector<std::vector<int>>& faces_){
        for (std::vector<int> face: faces_){
            std::stack<SpaceTreeNode> stack;
            stack.push(root);
            while (!stack.empty()){
                SpaceTreeNode node = stack.top();
                stack.pop();
                int i = node.index;
                AABBIntersectionCount++;
                if (AABBIntersection(node.centre, node.extents, vertices[face[0] - 1], vertices[face[3] - 1], vertices[face[6] - 1])){
                    if (nodes[i].is_leaf){
                        nodes[i].faces.push_back(face);
                    }
                    else{
                        for (int j = 0; j < 8; j++){
                            stack.push(nodes[(i+1)*8 + j]);
                        }
                    }
                }
            }
        }
    }

    void cull(){

    }
    

    std::vector<std::vector<int>> intersection(Ray ray){        
        // find the faces we need to test
        std::vector<std::vector<int>> tests = {};
        
        std::stack<SpaceTreeNode> stack;
        stack.push(root);
        while (!stack.empty()){
            SpaceTreeNode node = stack.top();
            stack.pop();
            int i = node.index;
            if (AABBIntersection(node.box[0], node.box[1], ray)){
                if (node.is_leaf){
                    for (auto& face: nodes[i].faces){
                        tests.push_back(face);
                    }
                }
                else{
                    for (int j = 0; j < 8; j++){
                        stack.push(nodes[(i+1)*8 + j]);
                    }
                }
            }
        }

        // return array with no duplicate faces
        std::set<std::vector<int>> s;
        unsigned size = tests.size();
        for(unsigned i = 0; i < size; i++){
            s.insert(tests[i]);
        }
        tests.assign(s.begin(), s.end());
        return tests;
    }
};