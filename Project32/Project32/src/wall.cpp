#include "wall.h"
#include <algorithm>
#include <cmath>

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

            float xPenetration = halfSize.x - std::abs(diff.x);
            float yPenetration = halfSize.y - std::abs(diff.y);
            float zPenetration = halfSize.z - std::abs(diff.z);

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

    return resolvedPos;
}

bool WallSystem::Raycast(const glm::vec3& origin, const glm::vec3& direction,
    float maxDistance, glm::vec3* hitPoint) const {
    float closestDistance = maxDistance;
    bool hit = false;

    for (const auto& wall : _walls) {
        glm::vec3 minBounds = wall.GetMinBounds();
        glm::vec3 maxBounds = wall.GetMaxBounds();

        // Ray-AABB intersection
        glm::vec3 invDir = 1.0f / direction;
        glm::vec3 t1 = (minBounds - origin) * invDir;
        glm::vec3 t2 = (maxBounds - origin) * invDir;

        glm::vec3 tMin = glm::min(t1, t2);
        glm::vec3 tMax = glm::max(t1, t2);

        float tNear = std::max(std::max(tMin.x, tMin.y), tMin.z);
        float tFar = std::min(std::min(tMax.x, tMax.y), tMax.z);

        if (tNear <= tFar && tNear > 0 && tNear < closestDistance) {
            closestDistance = tNear;
            hit = true;
            if (hitPoint) {
                *hitPoint = origin + direction * tNear;
            }
        }
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

    // Get rid of pillars for now
    //glm::vec3 pillarColor(0.6f, 0.6f, 0.4f);
    //AddWall(glm::vec3(-10, 2.5f, -10), glm::vec3(2, 5, 2), pillarColor);
    //AddWall(glm::vec3(10, 2.5f, -10), glm::vec3(2, 5, 2), pillarColor);
    //AddWall(glm::vec3(-10, 2.5f, 10), glm::vec3(2, 5, 2), pillarColor);
    //AddWall(glm::vec3(10, 2.5f, 10), glm::vec3(2, 5, 2), pillarColor);
}

void WallSystem::LoadLevel(const std::string& filename) {
    // Future implementation for loading wall data from file
    // For now, just create a default maze
    CreateMaze();
}