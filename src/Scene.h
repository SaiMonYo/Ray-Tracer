#pragma once

#include "Observable.h"
#include <memory>
#include <vector>
#include "Light.h"
#include "RayHit.h"

struct Scene{
    std::vector<std::shared_ptr<Observable>> objects;
    std::vector<std::shared_ptr<Light>> lights;
    Vector3 ambientColour;

    void add_object(std::shared_ptr<Observable> object){
        objects.push_back(object);
    }

    void add_light(std::shared_ptr<Light> light){
        lights.push_back(light);
    }

    RayHit closest_intersection(Ray ray){
        RayHit intersection;

        for(int i = 0; i < objects.size(); i++){
            std::shared_ptr<Observable> obj = objects[i];
            if(obj->intersect(ray, intersection)){
                intersection.index=i;
            }
        }
        return intersection;
    }
};