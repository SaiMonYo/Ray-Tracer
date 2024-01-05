#pragma once

#include "Observable.h"
#include <memory>
#include <vector>
#include "Light.h"
#include "RayHit.h"
#include "Camera.h"
#include "SkySphere.h"




/*
       +Z
        |      +Y
        |      /
        |     /
        |    /
        |   /
        |  /
        | /
        |/_______________________ +X

*/

struct Scene{
    // keeps track of objects and lights
    std::vector<std::shared_ptr<Observable>> objects;
    std::vector<std::shared_ptr<Light>> lights;
    std::shared_ptr<SkySphere> sky = nullptr;
    // only one ambient colour per scene
    Vector3 ambientColour;
    Camera cam;

    void add_object(std::shared_ptr<Observable> object){
        objects.push_back(object);
    }

    void add_light(std::shared_ptr<Light> light){
        lights.push_back(light);
    }

    void setup_camera(int width, int height){
        cam.set_screensize(width, height);
    }

    void closest_intersection(RayHit& intersection, const Ray ray){
        // go through each object and see if it intersects
        for(int i = 0; i < objects.size(); i++){
            objects[i]->intersect(ray, intersection);
        }
    }
};