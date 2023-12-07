#pragma once

#include "Vector.h"


struct RayHit{
    float distance;
    Vector3 point;
    Vector3 normal;
    int index = -1;
    float u;
    float v;
    float hu;
    float hv;

    RayHit(){distance = 1e8;}
};