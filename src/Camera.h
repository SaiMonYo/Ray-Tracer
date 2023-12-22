#pragma once

#include "MathUtils.h"
#include "Ray.h"

struct Camera{
    int width_px, height_px;
    float width_m, height_m;

    float fov = 45.0f;
    float aspect_ratio;

    Vector3 pivot;
    Vector3 position;
    Vector3 up;
    Vector3 right;
    Vector3 forward;

    float x_step_m, z_step_m;

    Camera(){
        position = Vector3(0,0,0);
        up = Vector3(0, 0, 1);
        right = Vector3(1, 0, 0);
        forward = Vector3(0, 1, 0);
    }


    void set_screensize(int width, int height){
        width_px = width;
        height_px = height;

        aspect_ratio = ((float) width)/((float) height);

        width_m = 2*tan(fov * M_PI / 360.0f);
        height_m = width_m / aspect_ratio;

        x_step_m = width_m / width_px;
        z_step_m = height_m / height_px;
    }

    void setup(Vector3 pos, Vector3 target){
        position = pos;
        look_at(target);
    }

    void set_position(Vector3 pos){
        position = pos;
    }

    void look_at(Vector3 target){
        pivot = target;
        Vector3 direction = Vector3::normalize(target - position);
        // float pitch = asin(direction.z);
        // float yaw = atan2(direction.y, direction.x);

        // Vector3 temp_forward;
        // temp_forward.x = cos(pitch) * cos(yaw);
        // temp_forward.y = sin(yaw) * cos(pitch);
        // temp_forward.z = sin(pitch);

        forward = direction;
        right = Vector3::normalize(Vector3::cross(forward, Vector3(0,0,1)));
        up = Vector3::normalize(Vector3::cross(right, forward));

        std::cout << "forward: " << position + forward << std::endl;
        std::cout << "right: " << position + right << std::endl;
        std::cout << "up: " << position + up << std::endl;
    }

    // return a ray through the pixel coordinate x, z
    Ray cast_ray(int x, int z){
        float x_pos = (x_step_m - width_m) / 2 + x * x_step_m;
        float z_pos = (z_step_m + height_m) / 2 - z * z_step_m;

        Vector3 ray_direction = x_pos * right + z_pos * up + forward;

        return Ray(position, Vector3::normalize(ray_direction));
    }
};