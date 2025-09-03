#include "common.h"
#include "game.h"
#include "input.h"
#include "renderer.h"
#include "camera.h"
#include "GL.h"

namespace Game {
    MovementState _stats;
    CameraManager manager;

    int cam1;
	int cam2;

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

    glm::vec3 GetFront() {
        glm::vec3 front;
        front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
        front.y = sin(glm::radians(_pitch));
        front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
        return glm::normalize(front);
    }

    glm::vec3 GetRight() {
        return glm::normalize(glm::cross(GetFront(), glm::vec3(0.0f, 1.0f, 0.0f)));
    }

    glm::vec3 GetUp() {
        return glm::normalize(glm::cross(GetRight(), GetFront()));
    }

    void GroundMove(const glm::vec3& wishdir, float wishspeed, float deltaTime) {
        float currentspeed = glm::dot(_velocity, wishdir);
        float addspeed = wishspeed - currentspeed;

        if (addspeed <= 0) return;

        float accelspeed = ACCEL * deltaTime * wishspeed;
        if (accelspeed > addspeed) {
            accelspeed = addspeed;
        }

        _velocity += wishdir * accelspeed;
    }

    void AirMove(const glm::vec3& wishdir, float wishspeed, float deltaTime) {
        float currentspeed = glm::dot(_velocity, wishdir);
        float addspeed = wishspeed - currentspeed;

        if (addspeed <= 0) return;

        float accelspeed = AIR_ACCEL * deltaTime * wishspeed;

        // Enhanced air strafing
        if (glm::length(wishdir) > 0.1f) {
            glm::vec3 horizontalVel = glm::vec3(_velocity.x, 0.0f, _velocity.z);
            float currentHorizontalSpeed = glm::length(horizontalVel);

            // Boost acceleration when strafing at high speeds
            if (currentHorizontalSpeed > MOVE_SPEED) {
                accelspeed *= STRAFE_ACCEL;
            }
        }

        if (accelspeed > addspeed) {
            accelspeed = addspeed;
        }

        _velocity += wishdir * accelspeed;
    }

    void ApplyFriction(float deltaTime) {
        float speed = glm::length(_velocity);
        if (speed < 0.1f) {
            _velocity = glm::vec3(0.0f);
            return;
        }

        float drop = 0.0f;
        float control = (speed < STOP_SPEED) ? STOP_SPEED : speed;
        drop += control * FRICTION * deltaTime;

        float newspeed = speed - drop;
        if (newspeed < 0) newspeed = 0;

        if (newspeed != speed) {
            newspeed /= speed;
            _velocity *= newspeed;
        }
    }

    void HandleJump() {
        bool canJump = _onGround || (_airTime < COYOTE_TIME && _wasOnGround);

        if (Input::KeyPressed(GLFW_KEY_SPACE) && canJump) {
            _velocity.y = JUMP_SPEED;
            _onGround = false;
            _stats.jumpCount++;

            // Preserve horizontal velocity for bunny hopping
            if (!_wasOnGround) {
                glm::vec3 horizontalVel = glm::vec3(_velocity.x, 0.0f, _velocity.z);
                horizontalVel *= BUNNY_HOP_SPEED_RETAIN;
                _velocity.x = horizontalVel.x;
                _velocity.z = horizontalVel.z;
            }
        }
    }

    void UpdateCamera() {
        auto& camera = manager.GetCamera(cam1);
		camera.SetTransform(Transform{
            .position = _position,
            .rotation = glm::vec3(_pitch, _yaw, 0.0f),
            .scale = glm::vec3(1.0f)
			});
	}

    void UpdateStats(float deltaTime) {
        _stats.sessionTime += deltaTime;

        float currentSpeed = GetSpeed();
        if (currentSpeed > _stats.maxSpeedReached) {
            _stats.maxSpeedReached = currentSpeed;
        }

        _stats.totalDistance += currentSpeed * deltaTime;
    }

    void Init() {
        Input::Init();

        std::cout << "=== Quake Movement Controls ===" << std::endl;
        std::cout << "WASD: Move" << std::endl;
        std::cout << "Space: Jump" << std::endl;
        std::cout << "Mouse: Look around" << std::endl;
        std::cout << "M: Toggle wireframe" << std::endl;
        std::cout << "F1: Toggle mouse lock" << std::endl;
        std::cout << "F11: Toggle fullscreen" << std::endl;
        std::cout << "H: Toggle HUD" << std::endl;
        std::cout << "ESC: Exit" << std::endl;
        std::cout << "===============================" << std::endl;

        cam1 = manager.AddCamera("Main");
        cam2 = manager.AddCamera("Secondary", Transform{
            .position = glm::vec3(0.0f, 10.0f, 0.0f),
            .rotation = glm::vec3(-90.0f, 0.0f, 0.0f),
            .scale = glm::vec3(1.0f)
			});
        manager.SetActiveCamera(cam1); // Find a better place for this
    }

    void Update(float deltaTime) {
        if (Input::KeyPressed(GLFW_KEY_ESCAPE)) {
            GL::SetWindowShouldClose(true);
        }
        if (Input::KeyPressed(GLFW_KEY_M)) {
            Renderer::ToggleWireframe();
        }
        if (Input::KeyPressed(GLFW_KEY_H)) {
            Renderer::ToggleDebugInfo();
        }
        if (Input::KeyPressed(GLFW_KEY_V)) {
            GL::SetVSync(!GL::_vsync);
        }

        if (Input::IsMouseLocked()) {
            glm::vec2 mouseDelta = Input::GetMouseDelta();
            _yaw += mouseDelta.x * MOUSE_SENSITIVITY;
            _pitch += mouseDelta.y * MOUSE_SENSITIVITY;

            // Constrain pitch
            _pitch = glm::clamp(_pitch, -89.0f, 89.0f);
        }

        glm::vec3 wishdir(0.0f);
        if (Input::KeyDown(GLFW_KEY_W)) wishdir += GetFront();
        if (Input::KeyDown(GLFW_KEY_S)) wishdir -= GetFront();
        if (Input::KeyDown(GLFW_KEY_A)) wishdir -= GetRight();
        if (Input::KeyDown(GLFW_KEY_D)) wishdir += GetRight();
		if (Input::KeyPressed(GLFW_KEY_V)) manager.SetToNextCamera();

        // Normalize to XZ plane for movement
        wishdir.y = 0.0f;
        if (glm::length(wishdir) > 0.0f) {
            wishdir = glm::normalize(wishdir);
        }

        float wishspeed = glm::length(wishdir) * MOVE_SPEED;

        _wasOnGround = _onGround;

        if (_onGround) {
            _groundTime += deltaTime;
            _airTime = 0.0f;
            GroundMove(wishdir, wishspeed, deltaTime);
            ApplyFriction(deltaTime);
        }
        else {
            _airTime += deltaTime;
            _groundTime = 0.0f;
            AirMove(wishdir, wishspeed, deltaTime);
        }

        HandleJump();

        if (!_onGround) {
            _velocity.y -= GRAVITY * deltaTime;
        }

        glm::vec3 horizontalVel = glm::vec3(_velocity.x, 0.0f, _velocity.z);
        float horizontalSpeed = glm::length(horizontalVel);
        if (horizontalSpeed > MAX_SPEED) {
            glm::vec3 normalizedHorizontal = glm::normalize(horizontalVel);
            _velocity.x = normalizedHorizontal.x * MAX_SPEED;
            _velocity.z = normalizedHorizontal.z * MAX_SPEED;
        }

        glm::vec3 oldPosition = _position;
        _position += _velocity * deltaTime;

        if (_position.y <= GROUND_Y) {
            _position.y = GROUND_Y;
            if (_velocity.y < 0) {
                _velocity.y = 0;
                if (!_onGround) {
                    _onGround = true;
                }
            }
        }
        else {
            _onGround = false;
        }

        UpdateStats(deltaTime);
		UpdateCamera();
    }

    glm::mat4 GetViewMatrix() {
		return manager.GetActiveCamera().GetViewMatrix();
    }

    glm::vec3 GetPosition() { return _position; }
    glm::vec3 GetVelocity() { return _velocity; }

    float GetSpeed() {
        glm::vec3 horizontalVel = glm::vec3(_velocity.x, 0.0f, _velocity.z);
        return glm::length(horizontalVel);
    }

    float GetYaw() { return _yaw; }
    float GetPitch() { return _pitch; }
    bool IsOnGround() { return _onGround; }
}