#pragma once

#include "Vector.h"

struct Ray{
    Vector3 origin;
    Vector3 direction;
    Vector3 inv_direction;

    Ray(Vector3 O, Vector3 D){
        origin = O;
        direction = D;
        // inverse direction is used for AABB intersection
        // cheaper to precompute
        inv_direction = Vector3::invert(D);
    }

    Vector3 at(float t) const{
        return origin + t * direction;
    }
};