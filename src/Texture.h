#pragma once

#include "QOI.h"
#include <vector>


struct Texture{
    bool implemented = false;
    virtual Vector3 get_colour(float u, float v)=0;
};

struct DefaultTexture : public Texture{
    DefaultTexture(){
        implemented = false;
    }
    Vector3 get_colour(float u, float v){
        return Vector3(0);
    }
};

struct ImageTexture : public Texture{
    int width;
    int height;
    std::vector<Vector3> pixels;

    ImageTexture(std::string filename){
        implemented = true;
        std::ifstream input(filename, std::ios::in|std::ios::binary);
        QOIReader qoi = QOIReader(input);
        width = qoi.width;
        height = qoi.height;
        qoi.read_all(pixels);
        input.close();
    }

    Vector3 get_colour(float u, float v){
        int x = (int)(u * width);
        // image is flipped in vertically
        int y = (int)((1-v) * height);
        return pixels.at(y * width + x);
    }
};