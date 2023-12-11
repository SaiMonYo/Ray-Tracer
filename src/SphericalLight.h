#pragma once

#include "Light.h"


struct SphericalLight: Light{
    float radius;

    SphericalLight(Vector3 loc, Vector3 clr, float inten, float r){
        position = loc;
        colour = clr;
        intensity = inten;
        radius = r;
    }

    Vector3 get_random_end(){
        return position + random_unit_vector() * radius;
    }

    // intensity decreases as function of surface area
    // 4 * pi * r^2
    Vector3 ilumination_at(float dist){
        return colour * intensity / (M_PI * 4 * dist * dist);
    }
};