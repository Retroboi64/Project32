#include "player.h"
#include "input.h"

glm::vec3 Player::GetFront() const {
    glm::vec3 front;
    front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    front.y = sin(glm::radians(_pitch));
    front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    return glm::normalize(front);
}

glm::vec3 Player::GetRight() const {
    return glm::normalize(glm::cross(GetFront(), glm::vec3(0.0f, 1.0f, 0.0f)));
}

glm::vec3 Player::GetUp() const {
    return glm::normalize(glm::cross(GetRight(), GetFront()));
}

void Player::GroundMove(const glm::vec3& wishdir, float wishspeed, float deltaTime) {
    float currentspeed = glm::dot(_velocity, wishdir);
    float addspeed = wishspeed - currentspeed;

    if (addspeed <= 0) return;

    float accelspeed = ACCEL * deltaTime * wishspeed;
    if (accelspeed > addspeed) {
        accelspeed = addspeed;
    }

    _velocity += wishdir * accelspeed;
}

void Player::AirMove(const glm::vec3& wishdir, float wishspeed, float deltaTime) {
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

void Player::ApplyFriction(float deltaTime) {
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

void Player::HandleJump() {
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

void Player::HandleMouseInput() {
    if (Input::IsMouseLocked()) {
        glm::vec2 mouseDelta = Input::GetMouseDelta();
        _yaw += mouseDelta.x * MOUSE_SENSITIVITY;
        _pitch += mouseDelta.y * MOUSE_SENSITIVITY;

        // Constrain pitch
        _pitch = glm::clamp(_pitch, -89.0f, 89.0f);
    }
}

void Player::UpdateStats(float deltaTime) {
    _stats.sessionTime += deltaTime;

    float currentSpeed = GetHorizontalSpeed();
    if (currentSpeed > _stats.maxSpeedReached) {
        _stats.maxSpeedReached = currentSpeed;
    }

    _stats.totalDistance += currentSpeed * deltaTime;
}

void Player::Update(float deltaTime) {
    HandleMouseInput();

    // Handle movement input
    glm::vec3 wishdir(0.0f);
    if (Input::KeyDown(GLFW_KEY_W)) wishdir += GetFront();
    if (Input::KeyDown(GLFW_KEY_S)) wishdir -= GetFront();
    if (Input::KeyDown(GLFW_KEY_A)) wishdir -= GetRight();
    if (Input::KeyDown(GLFW_KEY_D)) wishdir += GetRight();

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

    // Cap horizontal speed
    glm::vec3 horizontalVel = glm::vec3(_velocity.x, 0.0f, _velocity.z);
    float horizontalSpeed = glm::length(horizontalVel);
    if (horizontalSpeed > MAX_SPEED) {
        glm::vec3 normalizedHorizontal = glm::normalize(horizontalVel);
        _velocity.x = normalizedHorizontal.x * MAX_SPEED;
        _velocity.z = normalizedHorizontal.z * MAX_SPEED;
    }

    _position += _velocity * deltaTime;

    // Ground collision
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
}