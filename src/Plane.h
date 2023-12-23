#include "Observable.h"


struct Plane{
    Vector3 normal;
    Vector3 point;
    Material mat;

    Plane(Vector3 P, Vector3 N, Vector3 c){
        normal = N;
        point = P;
        mat.K_d = c;
    }
    Plane(Vector3 P, Vector3 N, std::string c){
        normal = N;
        point = P;
        mat.K_d = Vector3::to_colour(c);
    }

    bool intersect(Ray& ray, RayHit& hit){
        float denom = Vector3::dot(ray.direction, normal);
        if (fabs(denom) > EPSILON){
            float t = Vector3::dot((point - ray.origin), normal) / denom;
            if (t > EPSILON && t < hit.distance){
                hit.point = ray.at(t);
                hit.normal = normal;
                hit.distance = t;
                return true;
            }
        }
        return false;
    }
};