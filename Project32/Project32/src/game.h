#pragma once

#include "common.h"

// Forward declaration
class Player;

namespace Game {
    void Init();
    void Update(float deltaTime);
    glm::mat4 GetViewMatrix();

    // Player functions
    glm::vec3 GetPosition();
    glm::vec3 GetVelocity();
    float GetSpeed();
    float GetYaw();
    float GetPitch();
    bool IsOnGround();

    // Direct player access
    const Player& GetPlayer();
}