#pragma once

#include "Vector.h"


struct RayHit{
    float distance;
    Vector3 point;
    Vector3 normal;
    int index = -1;
    // object u, v coordinates
    float u;
    float v;
    // hit objects u, v coordinates
    // (i.e. objects triangle u, v)
    float hu;
    float hv;

    RayHit(){distance = 1e8;}
};