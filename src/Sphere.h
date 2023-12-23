#pragma once

#include "Ray.h"
#include "Observable.h"

struct Sphere: Observable{
    Vector3 centre;
    float radius;

    Vector3 max_vertex(){
        return centre + Vector3(radius);
    }

    Vector3 min_vertex(){
        return centre - Vector3(radius);
    }

    Vector3 centroid(){
        return centre;
    }

    Sphere(Vector3 O, float r, Vector3 c){
        centre = O;
        radius = r;
        mat.K_d = c;
    }

    Sphere(Vector3 O, float r, std::string c){
        centre = O;
        radius = r;
        mat.K_d = Vector3::to_colour(c);
    }

    Vector3 get_normal_at(const Vector3& point){
        return Vector3::normalize(point-centre);
    }

    bool intersect(Ray& ray, RayHit& hit){
        Vector3 oc = ray.origin - centre;
        float a = Vector3::length_squared(ray.direction);
        float half_b = Vector3::dot(oc, ray.direction);
        float c = Vector3::length_squared(oc) - radius*radius;

        float disc = half_b*half_b - a*c;
        if (disc < 0) return false;

        float sqrtd = sqrt(disc);
        float root = (-half_b - sqrtd) / a;

        if (root < 0 || hit.distance < root){
            root = (-half_b + sqrtd) / a;
            if (root < 0 || hit.distance < root){
                return false;
            }
        }

        hit.distance = root;
        hit.point = ray.at(root);
        hit.normal = get_normal_at(hit.point);
        return true;
    }
};