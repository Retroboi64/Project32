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

#include "common.h"
#include "wall.h"

void WallSystem::AddWall(const glm::vec3& pos, const glm::vec3& scale,
    const glm::vec3& color, const glm::vec3& rotation) {
    Wall wall;
    wall.position = pos;
    wall.scale = scale;
    wall.color = color;
    wall.rotation = rotation;
    _walls.push_back(wall);
}

void WallSystem::AddWalls(const std::vector<Wall>& walls) {
    _walls.insert(_walls.end(), walls.begin(), walls.end());
}

void WallSystem::Clear() {
    _walls.clear();
}

bool WallSystem::CheckPointCollision(const glm::vec3& point, float radius) const {
    for (const auto& wall : _walls) {
        glm::vec3 minBounds = wall.GetMinBounds() - glm::vec3(radius);
        glm::vec3 maxBounds = wall.GetMaxBounds() + glm::vec3(radius);

        if (point.x >= minBounds.x && point.x <= maxBounds.x &&
            point.y >= minBounds.y && point.y <= maxBounds.y &&
            point.z >= minBounds.z && point.z <= maxBounds.z) {
            return true;
        }
    }
    return false;
}

glm::vec3 WallSystem::ResolveCollision(const glm::vec3& oldPos, const glm::vec3& newPos,
    float radius) const {
    glm::vec3 resolvedPos = newPos;
    glm::vec3 movement = newPos - oldPos;

    // Check collision for each wall
    for (const auto& wall : _walls) {
        glm::vec3 minBounds = wall.GetMinBounds() - glm::vec3(radius);
        glm::vec3 maxBounds = wall.GetMaxBounds() + glm::vec3(radius);

        // Check if new position collides with this wall
        if (resolvedPos.x >= minBounds.x && resolvedPos.x <= maxBounds.x &&
            resolvedPos.y >= minBounds.y && resolvedPos.y <= maxBounds.y &&
            resolvedPos.z >= minBounds.z && resolvedPos.z <= maxBounds.z) {

            // Find the axis of minimum penetration
            glm::vec3 center = (minBounds + maxBounds) * 0.5f;
            glm::vec3 diff = resolvedPos - center;
            glm::vec3 halfSize = (maxBounds - minBounds) * 0.5f;

            // Calculate penetration on each axis
            float xPenetration = halfSize.x - std::abs(diff.x);
            float yPenetration = halfSize.y - std::abs(diff.y);
            float zPenetration = halfSize.z - std::abs(diff.z);

            // Only resolve if we're actually penetrating
            if (xPenetration > 0 && yPenetration > 0 && zPenetration > 0) {
                // Resolve along the axis with minimum penetration
                if (xPenetration < yPenetration && xPenetration < zPenetration) {
                    // Resolve along X axis
                    if (diff.x > 0) {
                        resolvedPos.x = maxBounds.x + 0.001f;
                    }
                    else {
                        resolvedPos.x = minBounds.x - 0.001f;
                    }
                }
                else if (yPenetration < zPenetration) {
                    // Resolve along Y axis  
                    if (diff.y > 0) {
                        resolvedPos.y = maxBounds.y + 0.001f;
                    }
                    else {
                        resolvedPos.y = minBounds.y - 0.001f;
                    }
                }
                else {
                    // Resolve along Z axis
                    if (diff.z > 0) {
                        resolvedPos.z = maxBounds.z + 0.001f;
                    }
                    else {
                        resolvedPos.z = minBounds.z - 0.001f;
                    }
                }
            }
        }
    }

    return resolvedPos;
}

bool WallSystem::Raycast(const glm::vec3& origin, const glm::vec3& direction,
    float maxDistance, glm::vec3* hitPoint) const {
    float closestDistance = maxDistance;
    bool hit = false;
    glm::vec3 closestHitPoint;

    glm::vec3 normalizedDir = glm::normalize(direction);

    for (const auto& wall : _walls) {
        glm::vec3 minBounds = wall.GetMinBounds();
        glm::vec3 maxBounds = wall.GetMaxBounds();

        // Ray-AABB intersection with improved algorithm
        glm::vec3 invDir = glm::vec3(
            abs(normalizedDir.x) > 0.0001f ? 1.0f / normalizedDir.x : 1000000.0f,
            abs(normalizedDir.y) > 0.0001f ? 1.0f / normalizedDir.y : 1000000.0f,
            abs(normalizedDir.z) > 0.0001f ? 1.0f / normalizedDir.z : 1000000.0f
        );

        glm::vec3 t1 = (minBounds - origin) * invDir;
        glm::vec3 t2 = (maxBounds - origin) * invDir;

        glm::vec3 tMin = glm::min(t1, t2);
        glm::vec3 tMax = glm::max(t1, t2);

        float tNear = std::max({ tMin.x, tMin.y, tMin.z, 0.0f });
        float tFar = std::min({ tMax.x, tMax.y, tMax.z });

        // Check if ray intersects the box and is closer than previous hits
        if (tNear <= tFar && tNear < closestDistance && tNear > 0.001f) {
            closestDistance = tNear;
            closestHitPoint = origin + normalizedDir * tNear;
            hit = true;
        }
    }

    if (hit && hitPoint) {
        *hitPoint = closestHitPoint;
    }

    return hit;
}

void WallSystem::CreateRoom(const glm::vec3& center, const glm::vec3& size) {
    float wallThickness = 0.5f;
    glm::vec3 wallColor(0.4f, 0.4f, 0.5f);

    // Floor
    //AddWall(center - glm::vec3(0, size.y / 2 + wallThickness / 2, 0),
    //    glm::vec3(size.x, wallThickness, size.z),
    //    glm::vec3(0.3f, 0.3f, 0.35f));

    // Ceiling
    AddWall(center + glm::vec3(0, size.y / 2 + wallThickness / 2, 0),
        glm::vec3(size.x, wallThickness, size.z),
        glm::vec3(0.25f, 0.25f, 0.3f));

    // Back wall (-Z)
    //AddWall(center - glm::vec3(0, 0, size.z / 2 + wallThickness / 2),
    //    glm::vec3(size.x, size.y, wallThickness),
    //    wallColor);

    // Front wall (+Z)
    //AddWall(center + glm::vec3(0, 0, size.z / 2 + wallThickness / 2),
    //    glm::vec3(size.x, size.y, wallThickness),
    //    wallColor);

    // Left wall (-X)
    //AddWall(center - glm::vec3(size.x / 2 + wallThickness / 2, 0, 0),
    //    glm::vec3(wallThickness, size.y, size.z),
    //    wallColor);

    // Right wall (+X)
    //AddWall(center + glm::vec3(size.x / 2 + wallThickness / 2, 0, 0),
    //    glm::vec3(wallThickness, size.y, size.z),
    //    wallColor);
}

void WallSystem::CreateMaze() {
    Clear();
    CreateRoom(glm::vec3(0, 2.5f, 0), glm::vec3(40, 5, 40));

    glm::vec3 wallColor(0.5f, 0.3f, 0.3f);
    AddWall(glm::vec3(-10, 2.5f, 0), glm::vec3(15, 5, 1), wallColor);
    AddWall(glm::vec3(10, 2.5f, -5), glm::vec3(15, 5, 1), wallColor);
    AddWall(glm::vec3(0, 2.5f, 10), glm::vec3(20, 5, 1), wallColor);
    AddWall(glm::vec3(-5, 2.5f, -10), glm::vec3(10, 5, 1), wallColor);
    AddWall(glm::vec3(0, 2.5f, -10), glm::vec3(1, 5, 15), wallColor);
    AddWall(glm::vec3(-15, 2.5f, 5), glm::vec3(1, 5, 10), wallColor);
    AddWall(glm::vec3(15, 2.5f, 0), glm::vec3(1, 5, 20), wallColor);
    AddWall(glm::vec3(5, 2.5f, 5), glm::vec3(1, 5, 10), wallColor);

    // Add some boundary walls to prevent falling off the map
    float boundary = 25.0f;
    float wallHeight = 10.0f;
    glm::vec3 boundaryColor(0.2f, 0.2f, 0.3f);

    // Boundary walls
    AddWall(glm::vec3(0, wallHeight / 2, boundary), glm::vec3(boundary * 2, wallHeight, 1), boundaryColor);  // North
    AddWall(glm::vec3(0, wallHeight / 2, -boundary), glm::vec3(boundary * 2, wallHeight, 1), boundaryColor); // South  
    AddWall(glm::vec3(boundary, wallHeight / 2, 0), glm::vec3(1, wallHeight, boundary * 2), boundaryColor);  // East
    AddWall(glm::vec3(-boundary, wallHeight / 2, 0), glm::vec3(1, wallHeight, boundary * 2), boundaryColor); // West
}

void WallSystem::LoadLevel(const std::string& filename) {
    // Future implementation for loading wall data from file
    // For now, just create a default maze
    CreateMaze();
}