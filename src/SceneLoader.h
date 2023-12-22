#pragma once

#include "Scene.h"
#include <cstring>


bool load_scene_file(const std::string& filename, Scene* scene){
    FILE* file = fopen(filename.c_str(), "r");
    if(file == NULL){
        std::cout << "Could not open file " << filename << std::endl;
        return false;
    }

    std::cout << "Loading scene file: " << filename << std::endl;

    char line[1024];
}