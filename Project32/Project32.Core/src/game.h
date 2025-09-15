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

#include "common.h"

class Player;
namespace Game {
    void Init();
    void Update(float deltaTime);
    glm::mat4 GetViewMatrix();

    // Player functions
    glm::vec3 GetPosition();
	glm::vec3 GetRotation();
    glm::vec3 GetVelocity();
    float GetSpeed();
    float GetYaw();
    float GetPitch();
    bool IsOnGround();

    // Direct player access
    const Player& GetPlayer();
}