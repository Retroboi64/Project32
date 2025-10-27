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

#ifndef WALL_H
#define WALL_H

#include "../common.h"

struct Wall {
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;
    glm::vec3 color;

    // Collision bounds (AABB)
    glm::vec3 GetMinBounds() const {
        return position - scale * 0.5f;
    }

    glm::vec3 GetMaxBounds() const {
        return position + scale * 0.5f;
    }

    Transform GetTransform() const {
        Transform t;
        t.position = position;
        t.scale = scale;
        t.rotation = rotation;
        return t;
    }
};

class WallSystem {
private:
    std::vector<Wall> _walls;
public:
    WallSystem() = default;
    void AddWall(const glm::vec3& pos, const glm::vec3& scale,
        const glm::vec3& color = glm::vec3(0.4f, 0.4f, 0.5f),
        const glm::vec3& rotation = glm::vec3(0.0f));
    void AddWalls(const std::vector<Wall>& walls);
	std::vector<Wall>& GetWalls() { return _walls; }
    void Clear();
    const std::vector<Wall>& GetWalls() const { return _walls; }
    bool CheckPointCollision(const glm::vec3& point, float radius = 0.5f) const;
    glm::vec3 ResolveCollision(const glm::vec3& oldPos, const glm::vec3& newPos,
        float radius = 0.5f) const;
    bool Raycast(const glm::vec3& origin, const glm::vec3& direction,
        float maxDistance, glm::vec3* hitPoint = nullptr) const;
    void CreateRoom(const glm::vec3& center, const glm::vec3& size);
    void CreateMaze();
    void LoadLevel(const std::string& filename);
};

#endif // !WALL_H