#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

struct MathUtils{
    static float clamp(float x, float min, float max){
        return std::min(std::max(x, min), max);
    }

    static float to_radians(float deg){
        return deg * M_PI / 180.0f;
    }

    static float to_degrees(float rad){
        return rad * 180.0f / M_PI;
    }
};