#pragma once


#include "AABB.h"
#include "RayHit.h"
#include "Triangle.h"
#include <iostream>
#include <vector>

#define uint unsigned int


struct SpaceTree{
    virtual bool intersection(const Ray& ray, RayHit& inter)=0;
};