#pragma once

#include "RayHit.h"
#include "Ray.h"
#include "Material.h"
#include "Texture.h"
#include <memory>

#define uint unsigned int


struct Observable{
    Material mat;
    std::shared_ptr<Texture> K_Atex = std::make_shared<DefaultTexture>();
    std::shared_ptr<Texture> Ntex = std::make_shared<DefaultTexture>();
    virtual bool intersect(Ray& r, RayHit& hit)=0;
    virtual inline Vector3 centroid()=0;
    virtual Vector3 max_vertex()=0;
    virtual Vector3 min_vertex()=0;
};