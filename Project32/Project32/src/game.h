#pragma once

#include "common.h"

namespace Game {
    struct MovementState {
        float maxSpeedReached = 0.0f;
        float totalDistance = 0.0f;
        int jumpCount = 0;
        float sessionTime = 0.0f;
    };

    extern const float MOVE_SPEED;
    extern const float MOUSE_SENSITIVITY;
    extern const float FRICTION;
    extern const float STOP_SPEED;
    extern const float ACCEL;
    extern const float AIR_ACCEL;
    extern const float JUMP_SPEED;
    extern const float GRAVITY;
    extern const float GROUND_Y;
    extern const float MAX_SPEED;
    extern const float STRAFE_ACCEL;
    extern const float BUNNY_HOP_SPEED_RETAIN;
    extern const float COYOTE_TIME;

    glm::vec3 GetFront();
    glm::vec3 GetRight();
    glm::vec3 GetUp();
    void GroundMove(const glm::vec3& wishdir, float wishspeed, float deltaTime);
    void AirMove(const glm::vec3& wishdir, float wishspeed, float deltaTime);
    void ApplyFriction(float deltaTime);
    void HandleJump();
    void UpdateStats(float deltaTime);
    void Init();
    void Update(float deltaTime);
    glm::mat4 GetViewMatrix();
    glm::vec3 GetPosition();
    glm::vec3 GetVelocity();
    float GetSpeed();
    float GetYaw();
    float GetPitch();
    bool IsOnGround();
}