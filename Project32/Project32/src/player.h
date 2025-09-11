#pragma once

#include "common.h"
#include "camera.h"

struct MovementStats {
    float maxSpeedReached = 0.0f;
    float totalDistance = 0.0f;
    int jumpCount = 0;
    float sessionTime = 0.0f;
};

class Player {
private:
	Camera _camera;
    MovementStats _stats;
	Transform _transform;

    glm::vec3 _position = glm::vec3(4.0f, 2.0f, 4.0f);
    glm::vec3 _velocity = glm::vec3(0.0f);
    float _yaw = -45.0f;
    float _pitch = -15.0f;
    bool _onGround = false;
    float _groundTime = 0.0f;
    float _airTime = 0.0f;
    bool _wasOnGround = false;

    const float MOVE_SPEED = 8.0f;
    const float MOUSE_SENSITIVITY = 0.1f;
    const float FRICTION = 8.0f;
    const float STOP_SPEED = 1.0f;
    const float ACCEL = 10.0f;
    const float AIR_ACCEL = 0.7f;
    const float JUMP_SPEED = 8.0f;
    const float GRAVITY = 20.0f;
    const float GROUND_Y = 0.5f;
    const float MAX_SPEED = 50.0f;
    const float STRAFE_ACCEL = 1.2f;
    const float BUNNY_HOP_SPEED_RETAIN = 0.95f;
    const float COYOTE_TIME = 0.1f;

    void GroundMove(const glm::vec3& wishdir, float wishspeed, float deltaTime);
    void AirMove(const glm::vec3& wishdir, float wishspeed, float deltaTime);
    void ApplyFriction(float deltaTime);
    void HandleJump();
    void HandleMouseInput();
    void UpdateStats(float deltaTime);
public:
    glm::vec3 GetPosition() const { return _position; }
	glm::vec3 GetRotation() const { return glm::vec3(_pitch, _yaw, 0.0f); }
    glm::vec3 GetVelocity() const { return _velocity; }
    float GetSpeed() const { return glm::length(_velocity); }
    float GetHorizontalSpeed() const {
        glm::vec3 horizontalVel = glm::vec3(_velocity.x, 0.0f, _velocity.z);
        return glm::length(horizontalVel);
    }
    float GetYaw() const { return _yaw; }
    float GetPitch() const { return _pitch; }
    bool IsOnGround() const { return _onGround; }
    const MovementStats& GetStats() const { return _stats; }

    void SetPosition(const glm::vec3& pos) { _position = pos; }
    void SetVelocity(const glm::vec3& vel) { _velocity = vel; }
    void SetYaw(float yaw) { _yaw = yaw; }
    void SetPitch(float pitch) { _pitch = glm::clamp(pitch, -89.0f, 89.0f); }

    glm::vec3 GetFront() const;
    glm::vec3 GetRight() const;
    glm::vec3 GetUp() const;

	Transform GetTransform() const { return _transform; }

    void Update(float deltaTime);
    void UpdateTransform();
	void UpdatePlayerCamera();

    std::unique_ptr<Camera> GetCamera() {
        // return std::make_unique<Camera>(_camera) Fix is moving the Camera object into the unique_ptr like below
        return std::make_unique<Camera>(std::move(_camera));  
    }
};