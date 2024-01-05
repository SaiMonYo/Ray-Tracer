#pragma once

#include "Vector.h"
#include "Material.h"


struct RayHit{
    float distance;
    Vector3 point;
    Vector3 normal;
    int index = -1;
    std::shared_ptr<Material> mat = nullptr;
    // object u, v coordinates
    float u;
    float v;
    // hit objects u, v coordinates
    // (i.e. objects triangle u, v)
    float hu;
    float hv;

    RayHit(){distance = FINF;}
};