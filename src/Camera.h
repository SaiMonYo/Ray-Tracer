#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include "Ray.h"

struct Camera{
    int width_px, height_px;
    float width_m, height_m;

    float fov = 45.0f;
    float aspect_ratio;

    float x_step_m, z_step_m;

    Camera(int width, int height){
        width_px = width;
        height_px = height;

        aspect_ratio = ((float) width)/((float) height);

        width_m = 2*tan(fov * M_PI / 360.0f);
        height_m = width_m / aspect_ratio;

        x_step_m = width_m / width_px;
        z_step_m = height_m / height_px;
    }

    // return a ray through the pixel coordinate x, z
    Ray cast_ray(int x, int z){
        float x_pos = (x_step_m - width_m) / 2 + x * x_step_m;
        float z_pos = (z_step_m + height_m) / 2 - z * z_step_m;

        return Ray(Vector3(0,0,0), Vector3::normalize(Vector3(x_pos, 1, z_pos)));
    }
};