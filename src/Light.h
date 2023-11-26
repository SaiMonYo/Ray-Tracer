#pragma once

#include "Vector.h"

struct Light{
    Vector3 colour;
    float intensity;
    Vector3 position;

    virtual Vector3 get_random_end()=0;
    virtual Vector3 ilumination_at(float dist)=0;
};