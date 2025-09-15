/*
 * This file is part of Project32 - A compact yet powerful and flexible C++ Game Engine
 * Copyright (c) 2025 Patrick Reese (Retroboi64)
 *
 * Licensed under MIT with Attribution Requirements
 * See LICENSE file for full terms
 * GitHub: https://github.com/Retroboi64/Project32
 *
 * This header must not be removed from any source file.
 */

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