#pragma once

#include "Scene.h"
#include "QOI.h"
#include "Camera.h"
#include <chrono>



struct Renderer{
    int width, height;
    QOIWriter* out;
    Scene world;
    int max_bounces = 2;
    int shadow_rays = 10;
    int spp = 5;

    Renderer(QOIWriter* output, int w, int h, Scene s){
        out = output;
        width = w;
        height = h;
        world = s;
    }

    // trace ray through scene and find information of intersection
    Vector3 trace(Ray& ray){
        RayHit closest = world.closest_intersection(ray);

        int index = closest.index;
        if (index < 0){
            return Vector3(0);
        }

        //ray.origin = closest.point + closest.normal * EPSILON;
        //ray.direction = random_hemisphere_vector(closest.normal);
        return illuminate(index, closest.point, closest.normal, ray.origin, closest.u, closest.v);
    }

    // illuminate a point on an object
    // using Blinn-Phong Shading model
    Vector3 illuminate(int index, Vector3 P, Vector3 N, Vector3 O, float u, float v){
        // colour of point to be returned
        Vector3 colour = Vector3(0,0,0);

        Material mat = world.objects[index]->mat;
        Vector3 V = Vector3::normalize(O-P);
        Vector3 K_d = mat.K_d;

        // if object has a diffuse texture sample it
        if (world.objects[index]->K_Atex->implemented){
            K_d = world.objects[index]->K_Atex->get_colour(u, v);
        }
        Vector3 K_s = mat.K_s;
        Vector3 I_a = world.ambientColour;
        int alpha = mat.N_s;

        // ambient lighting
        colour += K_d * I_a;

        // calculate diffuse and specular components for each light
        for (int i = 0; i < world.lights.size(); i++){
            std::shared_ptr<Light> light = world.lights[i];
            float dist = Vector3::length(light->position - P);
            Vector3 C_spec = light->colour;
            Vector3 I = light->ilumination_at(dist);

            Vector3 L = (light->position - P) / dist;

            // Diffuse
            float theta = Vector3::dot(N, L);
            colour += K_d * fmax(theta, 0) * I;

            // Specular
            Vector3 R = Vector3::normalize(N * 2 * theta - L);
            float phi = Vector3::dot(R, V);
            colour += C_spec * K_s * I * pow(fmax(phi, 0), alpha);
        }
        return colour;
    }

    void render(){
        // timer for render time
        auto start = std::chrono::high_resolution_clock::now();
        Camera cam = Camera(width, height);

        // used for outputting the renders current %
        int tenpercent = height/10;
        std::cout << "Rendering..." << std::endl;

        // iterate through the pixel coordinates
        for (int z = 0; z < height; z++){
            for (int x = 0; x < width; x++){
                // get the ray from the camera goinf through that coordinate
                Ray r = cam.cast_ray(x,z);
                Vector3 lin_rgb = trace(r);
                // write value to file
                out->write_pixel(lin_rgb*255);
            }
            if (z % tenpercent == 0){
                std::cout << (100.0*z)/(height) << "% complete" << std::endl;
            }
        }
        std::cout << "100% complete" << std::endl;
        // output the render time
        auto end = std::chrono::high_resolution_clock::now();
        unsigned long long r_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        // less than 30s output ms, else seconds
        if (r_time_ms < 30000){
            std::cout << "Render time: " << r_time_ms << "ms" << std::endl;
        }
        else{
            std::cout << "Render time: " << (r_time_ms / 1000) << "s" << std::endl;
        }
    }

    Vector3 tonemap(Vector3 lin_rgb){
        double inv_gamma = 1./2.2;
        double a = 2;
        double b = 1.3;

        Vector3 powered = Vector3::power(lin_rgb, b);
        Vector3 display = powered * ( Vector3::invert(powered + pow(0.5/a, b)) );

        return Vector3::power(display, inv_gamma);
    }
};