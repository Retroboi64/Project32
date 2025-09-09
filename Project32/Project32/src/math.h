#pragma once

#include "glm/glm.hpp"

// TODO: Add more math utilities as needed
namespace Math {
    inline float Lerp(float a, float b, float t) {
        return a + t * (b - a);
    }

    inline glm::vec3 Lerp(const glm::vec3& a, const glm::vec3& b, float t) {
        return a + t * (b - a);
    }
}