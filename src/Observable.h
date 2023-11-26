#pragma once

#include "RayHit.h"
#include "Ray.h"
#include "Material.h"
#include "Texture.h"
#include <memory>



struct Observable{
    Material mat;
    std::shared_ptr<Texture> K_Atex;
    std::shared_ptr<Texture> Ntex;
    virtual bool intersect(Ray& r, RayHit& hit)=0;
};