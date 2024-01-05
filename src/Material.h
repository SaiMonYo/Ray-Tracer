#pragma once

#include "texture.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <memory>


struct Material{
    Vector3 K_a = Vector3(1);
    Vector3 K_d = Vector3(1);
    Vector3 K_s = Vector3(1);
    float N_s = 1;
    float N_i = 1;
    float d = 1;
    std::shared_ptr<Texture> K_Dtex = nullptr;
};



struct DefaultMaterial: public Material{
    DefaultMaterial(std::string colour){
        K_a = Vector3(1);
        K_d = Vector3::to_colour(colour);
        K_s = Vector3(1);
        N_s = 10.0;
        N_i = 0.0;
        d = 1.0;
    }
};

Material parse_material(const std::string& filename){
    Material mat;

    std::ifstream file(filename);
    if(!file.is_open()){
        std::cerr << "Could not open file " << filename << std::endl;
        return mat;
    }

    std::string line;
    while (std::getline(file, line)){
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string component;
        iss >> component;

        if (component == "Ka"){
            Vector3 v;
            iss >> v.x >> v.y >> v.z;
            mat.K_a = v;
        }
        else if (component == "Kd"){
            Vector3 v;
            iss >> v.x >> v.y >> v.z;
            mat.K_d = v;
        }
        else if (component == "Ks"){
            Vector3 v;
            iss >> v.x >> v.y >> v.z;
            mat.K_s = v;
        }
        else if (component == "Ns"){
            float v;
            iss >> v;
            mat.N_s = v;
        }
        else if (component == "Ni"){
            float v;
            iss >> v;
            mat.N_i = v;
        }
        else if (component == "d"){
            float v;
            iss >> v;
            mat.d = v;
        }
        else{}
    }
    return mat;
}

struct MtlMaterial: public Material{
    MtlMaterial(const std::string& filename){
        Material mat = parse_material(filename);
        K_a = mat.K_a;
        K_d = mat.K_d;
        K_s = mat.K_s;
        N_s = mat.N_s;
        N_i = mat.N_i;
        d = mat.d;
    }
};