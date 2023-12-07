#pragma once

#include "AABB.h"
#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <set>

struct SpaceTreeNode{
    Vector3 box[2];
    Vector3 centre;
    Vector3 extents;
    int index = -1;
    bool is_leaf = false;
    bool is_dead = false;
    std::vector<std::vector<int>> faces;
    std::vector<int> children;

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
    int starting_max_index = 0;

    Octree(){
    }

    Octree(Vector3 boundingBox_[2], std::vector<std::vector<int>>& faces_, std::vector<Vector3>& vertices_, uint8_t depth_){
        // add the meshes bounding box to the roots 
        root = SpaceTreeNode(boundingBox_[0], boundingBox_[1], -1);
        root.is_dead = false;
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
        std::vector<int> face_count(nodes.size(), 0);
        for (std::vector<int> face: faces_){
            std::stack<SpaceTreeNode> stack;
            for (int j = 0; j < 8; j++){
                stack.push(nodes[j]);
            }
            while (!stack.empty()){
                SpaceTreeNode node = stack.top();
                stack.pop();
                int i = node.index;
                if (AABBIntersection(node.centre, node.extents, Triangle(vertices[face[0] - 1], vertices[face[3] - 1], vertices[face[6] - 1], 1))){
                    face_count[i]++;
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
        for (int i = 0; i < nodes.size(); i++){
            if (face_count[i] == 0){
                nodes[i].is_dead = true;
            }
        }
        for (int i = 0; i < nodes.size(); i++){
            if (nodes[i].is_dead || nodes[i].is_leaf){
                continue;
            }
            int base_index = (i+1) * 8;
            for (int j = 0; j < 8; j++){
                if (!nodes[base_index + j].is_dead){
                    nodes[i].children.push_back(base_index + j);
                }
            }
        }
        starting_max_index = 8;

        
        bool done = false;
        while (!done){
            done = true;
            for (int i = 0; i < nodes.size(); i++){
                if (nodes[i].is_dead || nodes[i].is_leaf){
                    continue;
                }
                if (nodes[i].children.size() == 0){
                    nodes[i].is_dead = true;
                    done = false;
                }
            }
        }
        
        
        std::vector<int> map(nodes.size(), -1);
        std::vector<SpaceTreeNode> new_nodes;
        for (int i = 0; i < nodes.size(); i++){
            if (!nodes[i].is_dead){
                map[i] = new_nodes.size();
                new_nodes.push_back(nodes[i]);
            }
        }
        for (int i = 0; i < new_nodes.size(); i++){
            std::vector<int> new_children;
            for (int j = 0; j < new_nodes[i].children.size(); j++){
                int index = map[new_nodes[i].children[j]];
                if (index > -1){
                    new_children.push_back(index);
                }
            }
            new_nodes[i].children = new_children;
        }
        nodes.clear();
        nodes = new_nodes;

        std::cout << nodes.size() << std::endl;

        for (int i = 0; i < starting_max_index; i++){
            if (map[i] == -1){
                starting_max_index = i;
                break;
            }
        }

        for (int i = 0; i < nodes.size(); i++){
            nodes[i].index = i;
        }
    }
    

    std::vector<std::vector<int>> intersection(Ray ray){       
        if (!AABBIntersection(root.box[0], root.box[1], ray)){
            return {};
        }
        // find the faces we need to test
        std::vector<std::vector<int>> tests = {};
        
        std::stack<int> stack;
        for (int j = 0; j < starting_max_index; j++){
            stack.push(j);
        }
        while (!stack.empty()){
            int i = stack.top();
            stack.pop();
            SpaceTreeNode node = nodes[i];
            if (node.is_dead){
                continue;
            }
            if (AABBIntersection(node.box[0], node.box[1], ray)){
                if (node.is_leaf){
                    for (auto& face: nodes[i].faces){
                        tests.push_back(face);
                    }
                }
                else{
                    for (int j: nodes[i].children){
                        stack.push(j);
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