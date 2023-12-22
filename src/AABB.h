#pragma once

#include "Triangle.h"

unsigned long long AABBIntersectionCount = 0;

struct AABB{
	Vector3 min, max;

	AABB(){
		min = Vector3(1e8f);
		max = Vector3(-1e8f);
	}

	constexpr inline Vector3& operator[](int ind){
        if (ind == 0){
			return min;
		}
		return max;
    }

    constexpr inline const Vector3& operator[](int ind) const {
        if (ind == 0){
			return min;
		}
		return max;
    }

	void fix(Triangle& tri){
		for (int i = 0; i < 3; i++){
			min = Vector3::min(min, tri.vertices[i]);
			max = Vector3::max(max, tri.vertices[i]);
		}
	}

	void fix(AABB& box){
		min = Vector3::min(min, box.min);
		max = Vector3::max(max, box.max);
	}

	void fix(Vector3& v){
		min = Vector3::min(min, v);
		max = Vector3::max(max, v);
	}
	
	float area(){
		Vector3 e = max - min;
		return 2.0f * (e.x * e.y + e.x * e.z + e.y * e.z);
	}

	Vector3 extents(){
		return max - min;
	}

	Vector3 center(){
		return (min + max) * 0.5f;
	}

	Vector3 offset(Vector3& p){
		Vector3 o = p - min;
		if (max.x > min.x) o.x /= max.x - min.x;
		if (max.y > min.y) o.y /= max.y - min.y;
		if (max.z > min.z) o.z /= max.z - min.z;
		return o;
	}
};

/*
inline float AABBIntersection(const AABB& aabb, const Ray& ray){
    AABBIntersectionCount++;
	Vector3 min = aabb.min;
	Vector3 max = aabb.max;
    // ray AABB intersection using Nvidia's ray slab intersection algorithm
    Vector3 f = (min - ray.origin) * ray.inv_direction;
    Vector3 n = (max - ray.origin) * ray.inv_direction;
    Vector3 tmin = Vector3::min(f, n);
    Vector3 tmax = Vector3::max(f, n);

    float t1 = Vector3::min_component(tmax);
    float t0 = Vector3::max_component(tmin);

    return (t1 >= t0) ? (t0 != FINF ? t0 : t1) : FINF;
}
*/


inline float AABBIntersection(const AABB& aabb, const Ray& ray){
	AABBIntersectionCount++;
	Vector3 bmin = aabb.min;
	Vector3 bmax = aabb.max;
    float tx1 = (bmin.x-ray.origin.x) * ray.inv_direction.x, tx2 = (bmax.x-ray.origin.x) * ray.inv_direction.x;
    float tmin = fmin( tx1, tx2 ), tmax = fmax( tx1, tx2 );
    float ty1 = (bmin.y-ray.origin.y) * ray.inv_direction.y, ty2 = (bmax.y-ray.origin.y) * ray.inv_direction.y;
    tmin = fmax( tmin, fmin( ty1, ty2 ) ), tmax = fmin( tmax, fmax( ty1, ty2 ) );
    float tz1 = (bmin.z-ray.origin.z) * ray.inv_direction.z, tz2 = (bmax.z-ray.origin.z) * ray.inv_direction.z;
    tmin = fmax( tmin, fmin( tz1, tz2 ) ), tmax = fmin( tmax, fmax( tz1, tz2 ) );
    if (tmax >= tmin && tmax > 0) return tmin; else return 1e30f;
}

/*
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
}*/

// https://github.com/juj/MathGeoLib/blob/master/src/Geometry/Triangle.cpp#L715
inline bool AABBIntersection(const Vector3& cent, const Vector3& e, Triangle& tri){
/** The AABB-Triangle test implementation is based on the pseudo-code in
	Christer Ericson's Real-Time Collision Detection, pp. 169-172. It is
	practically a standard SAT test. */
    Vector3 a = tri.vertices[0];
    Vector3 b = tri.vertices[1];
    Vector3 c = tri.vertices[2];
	Vector3 tMin = Vector3::min(Vector3::min(a, b), c);
	Vector3 tMax = Vector3::max(Vector3::max(a, b), c);

    Vector3 minPoint = cent - e;
    Vector3 maxPoint = cent + e;

	if (tMin.x >= maxPoint.x || tMax.x <= minPoint.x
		|| tMin.y >= maxPoint.y || tMax.y <= minPoint.y
		|| tMin.z >= maxPoint.z || tMax.z <= minPoint.z)
		return false;

	Vector3 center = (minPoint + maxPoint) * 0.5f;
	Vector3 h = maxPoint - center;

	const Vector3 t[3] = { b-a, c-a, c-b };

	Vector3 ac = a-center;

	Vector3 n = Vector3::cross(t[0], t[1]);
	float s = Vector3::dot(n, ac);
    float r = fabs(Vector3::dot(Vector3::abs(n), h));
	if (fabs(s) >= r)
		return false;

	const Vector3 at[3] = {Vector3::abs(t[0]), Vector3::abs(t[1]), Vector3::abs(t[2]) };

	Vector3 bc = b-center;
	Vector3 cc = c-center;

	// SAT test all cross-axes.
	// The following is a fully unrolled loop of this code, stored here for reference:
	/*
	float d1, d2, a1, a2;
	const Vector3 e[3] = { DIR_VEC(1, 0, 0), DIR_VEC(0, 1, 0), DIR_VEC(0, 0, 1) };
	for(int i = 0; i < 3; ++i)
		for(int j = 0; j < 3; ++j)
		{
			Vector3 axis = Cross(e[i], t[j]);
			ProjectToAxis(axis, d1, d2);
			aabb.ProjectToAxis(axis, a1, a2);
			if (d2 <= a1 || d1 >= a2) return false;
		}
	*/

	// eX <cross> t[0]
	float d1 = t[0].y * ac.z - t[0].z * ac.y;
	float d2 = t[0].y * cc.z - t[0].z * cc.y;
	float tc = (d1 + d2) * 0.5f;
	r = fabs(h.y * at[0].z + h.z * at[0].y);
	if (r + fabs(tc - d1) < fabs(tc))
		return false;

	// eX <cross> t[1]
	d1 = t[1].y * ac.z - t[1].z * ac.y;
	d2 = t[1].y * bc.z - t[1].z * bc.y;
	tc = (d1 + d2) * 0.5f;
	r = fabs(h.y * at[1].z + h.z * at[1].y);
	if (r + fabs(tc - d1) < fabs(tc))
		return false;

	// eX <cross> t[2]
	d1 = t[2].y * ac.z - t[2].z * ac.y;
	d2 = t[2].y * bc.z - t[2].z * bc.y;
	tc = (d1 + d2) * 0.5f;
	r = fabs(h.y * at[2].z + h.z * at[2].y);
	if (r + fabs(tc - d1) < fabs(tc))
		return false;

	// eY <cross> t[0]
	d1 = t[0].z * ac.x - t[0].x * ac.z;
	d2 = t[0].z * cc.x - t[0].x * cc.z;
	tc = (d1 + d2) * 0.5f;
	r = fabs(h.x * at[0].z + h.z * at[0].x);
	if (r + fabs(tc - d1) < fabs(tc))
		return false;

	// eY <cross> t[1]
	d1 = t[1].z * ac.x - t[1].x * ac.z;
	d2 = t[1].z * bc.x - t[1].x * bc.z;
	tc = (d1 + d2) * 0.5f;
	r = fabs(h.x * at[1].z + h.z * at[1].x);
	if (r + fabs(tc - d1) < fabs(tc))
		return false;

	// eY <cross> t[2]
	d1 = t[2].z * ac.x - t[2].x * ac.z;
	d2 = t[2].z * bc.x - t[2].x * bc.z;
	tc = (d1 + d2) * 0.5f;
	r = fabs(h.x * at[2].z + h.z * at[2].x);
	if (r + fabs(tc - d1) < fabs(tc))
		return false;

	// eZ <cross> t[0]
	d1 = t[0].x * ac.y - t[0].y * ac.x;
	d2 = t[0].x * cc.y - t[0].y * cc.x;
	tc = (d1 + d2) * 0.5f;
	r = fabs(h.y * at[0].x + h.x * at[0].y);
	if (r + fabs(tc - d1) < fabs(tc))
		return false;

	// eZ <cross> t[1]
	d1 = t[1].x * ac.y - t[1].y * ac.x;
	d2 = t[1].x * bc.y - t[1].y * bc.x;
	tc = (d1 + d2) * 0.5f;
	r = fabs(h.y * at[1].x + h.x * at[1].y);
	if (r + fabs(tc - d1) < fabs(tc))
		return false;

	// eZ <cross> t[2]
	d1 = t[2].x * ac.y - t[2].y * ac.x;
	d2 = t[2].x * bc.y - t[2].y * bc.x;
	tc = (d1 + d2) * 0.5f;
	r = fabs(h.y * at[2].x + h.x * at[2].y);
	if (r + fabs(tc - d1) < fabs(tc))
		return false;

	// No separating axis exists, the AABB and triangle intersect.
	return true;
}