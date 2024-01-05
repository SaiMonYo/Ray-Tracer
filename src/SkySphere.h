#pragma once

#include "Texture.h"
#include "Vector.h"
#include "MathUtils.h"


struct SkySphere {
    std::shared_ptr<ImageTexture> texture;

    SkySphere(std::shared_ptr<ImageTexture> texture) : texture(texture) {}

    Vector3 get_colour(Vector3 direction) {
        float u = atan2(direction.x, direction.y) / (2 * M_PI) + 0.5;
        float v = direction.z * 0.5 + 0.5;
        return texture->get_colour(u, v);
    }
};