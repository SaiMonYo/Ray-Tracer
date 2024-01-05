#pragma once

#include "RayHit.h"
#include "Observable.h"
#include "Ray.h"

unsigned long long triangle_count = 0;

struct Triangle: public Observable{
    Vector3 vertices[3];
    int face_index;

    Triangle(Vector3 v0, Vector3 v1, Vector3 v2, int index){
        vertices[0] = v0;
        vertices[1] = v1;
        vertices[2] = v2;
        face_index = index;
    }

    inline Vector3 centroid(){
        return (vertices[0] + vertices[1] + vertices[2]) / 3.0;
    }

    Vector3 max_vertex(){
        return Vector3::max(vertices[0], Vector3::max(vertices[1], vertices[2]));
    }

    Vector3 min_vertex(){
        return Vector3::min(vertices[0], Vector3::min(vertices[1], vertices[2]));
    }

    // moller trumbore algorithm
    inline bool intersect(const Ray& ray, RayHit& inter){
        triangle_count++;
        Vector3 v0 = vertices[0];
        Vector3 v1 = vertices[1];
        Vector3 v2 = vertices[2];
        Vector3 v0v1 = v1 - v0;
        Vector3 v0v2 = v2 - v0;
        Vector3 pvec = Vector3::cross(ray.direction, v0v2);
        float det = Vector3::dot(v0v1,pvec);

        float invdet = 1.0 / det;
        Vector3 tvec = ray.origin - v0;
        float u = Vector3::dot(tvec, pvec) * invdet;
        
        if (u < 0 || u > 1){
            return false;
        }

        Vector3 qvec = Vector3::cross(tvec, v0v1);
        float v = Vector3::dot(ray.direction, qvec) * invdet;

        if (v < 0 || u + v > 1){
            return false;
        }

        float t = Vector3::dot(v0v2,qvec) * invdet;
        if (t > EPSILON && t < inter.distance){
            inter.distance = t;
            inter.index = face_index;
            inter.hu = u;
            inter.hv = v;
            return true;
        }
        return false;
    }
};