#include "Triangle.h"

unsigned long long AABBIntersectionCount = 0;

inline float AABBIntersection(const Vector3& min, const Vector3& max, const Ray& ray){
    AABBIntersectionCount++;
    // ray AABB intersection using Nvidia's ray slab intersection algorithm
    Vector3 f = (min - ray.origin) * ray.inv_direction;
    Vector3 n = (max - ray.origin) * ray.inv_direction;
    Vector3 tmin = Vector3::min(f, n);
    Vector3 tmax = Vector3::max(f, n);

    float t1 = Vector3::min_component(tmax);
    float t0 = Vector3::max_component(tmin);

    return (t1 >= t0) ? (t0 > 0.f ? t0 : t1) : -1.0;
}

inline bool AABBIntersection(const Vector3& center, const Vector3& e, Triangle& tri){
    // crude AABB-triangle intersection
    // creates a bounding box around triangle and checks for intersection with AABB
    // quick but quite a few false positives
    // speeds up octree creation but slows down raytracing
    Vector3 v0 = tri.vertices[0];
    Vector3 v1 = tri.vertices[1];
    Vector3 v2 = tri.vertices[2];
    Vector3 top = Vector3::max(v0, Vector3::max(v1,v2));
    Vector3 bottom = Vector3::min(v0, Vector3::min(v1, v2));
    Vector3 center1 = (top + bottom) / 2.0f;
    Vector3 e1 = (top - center1);

    if (std::fabs(center.x - center1.x) > e.x + e1.x) return false;
    if (std::fabs(center.y - center1.y) > e.y + e1.y) return false;
    if (std::fabs(center.z - center1.z) > e.z + e1.z) return false;

    Vector3 normal = Vector3::cross((v1 - v0),(v2 - v0));
    float r = e.x * fabs(normal.x) + e.y * fabs(normal.y) + e.z * fabs(normal.z);
    float s = Vector3::dot(normal, (center - v0));

    return (fabs(r) > s);
}