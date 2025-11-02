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

#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>

#define MAX_BONE_INFLUENCE 4 

struct Vertex {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 normal = glm::vec3(0.0f);
    glm::vec2 uv = glm::vec2(0.0f);
    glm::vec2 texCoord = glm::vec2(0.0f);
    glm::vec3 tangent = glm::vec3(0.0f);
    glm::vec3 bitangent = glm::vec3(0.0f);
    glm::vec3 color = glm::vec3(1.0f);
    int m_BoneIDs[MAX_BONE_INFLUENCE] = { 0, 0, 0, 0 };
    float m_Weights[MAX_BONE_INFLUENCE] = { 0.0f, 0.0f, 0.0f, 0.0f };

    Vertex(
        const glm::vec3& pos,
        const glm::vec3& norm,
        const glm::vec2& tex,
        const glm::vec3& col = glm::vec3(1.0f))
        : position(pos), normal(norm), texCoord(tex), color(col)
    {
    }
};

#endif // VERTEX_H