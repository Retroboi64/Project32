#include "player.h"
#include "input.h"

glm::vec3 Player::GetFront() const {
    float x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    float y = sin(glm::radians(_pitch));
    float z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    return glm::normalize(glm::vec3(x, y, z));
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

    if (accelspeed > AIR_ACCEL * deltaTime * AIR_ACCEL_CAP) {
        accelspeed = AIR_ACCEL * deltaTime * AIR_ACCEL_CAP;
    }

    if (glm::length(wishdir) > 0.1f) {
        glm::vec3 horizontalVel = glm::vec3(_velocity.x, 0.0f, _velocity.z);
        float currentHorizontalSpeed = glm::length(horizontalVel);

        glm::vec3 horizontalWishdir = glm::normalize(glm::vec3(wishdir.x, 0.0f, wishdir.z));
        glm::vec3 horizontalVelNorm = currentHorizontalSpeed > 0.1f ?
            glm::normalize(horizontalVel) : glm::vec3(0.0f);

        float strafeDot = glm::dot(horizontalWishdir, horizontalVelNorm);

        if (currentHorizontalSpeed > MOVE_SPEED && abs(strafeDot) < 0.85f) {
            accelspeed *= STRAFE_ACCEL;
        }
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
        _lastJumpTime = _stats.sessionTime;

        if (!_wasOnGround && _airTime < COYOTE_TIME) {
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

        _pitch = glm::clamp(_pitch, -89.0f, 89.0f);
    }
}

void Player::HandleMovementInput() {
    _forwardInput = 0.0f;
    _rightInput = 0.0f;

    if (Input::KeyDown(GLFW_KEY_W)) _forwardInput += 1.0f;
    if (Input::KeyDown(GLFW_KEY_S)) _forwardInput -= 1.0f;
    if (Input::KeyDown(GLFW_KEY_D)) _rightInput += 1.0f;
    if (Input::KeyDown(GLFW_KEY_A)) _rightInput -= 1.0f;

    _forwardInput = glm::clamp(_forwardInput, -1.0f, 1.0f);
    _rightInput = glm::clamp(_rightInput, -1.0f, 1.0f);
}

// FOR FUTURE USE: Wall surfing mechanics/collision detection
//WallInfo Player::CheckWallCollision(const glm::vec3& direction) {
//    WallInfo wallInfo;
//
//    glm::vec3 rayStart = _position + glm::vec3(0.0f, 0.5f, 0.0f); // Eye level
//    glm::vec3 rayEnd = rayStart + direction * WALL_DETECTION_DISTANCE;
//
//    wallInfo.hitWall = false;
//    wallInfo.normal = glm::vec3(0.0f);
//    wallInfo.distance = WALL_DETECTION_DISTANCE;
//
//    return wallInfo;
//}

// FOR FUTURE USE: Wall surfing mechanics
//void Player::HandleWallSurfing(const WallInfo& wallInfo, float deltaTime) {
//    if (!wallInfo.hitWall || _onGround) return;
//
//    glm::vec3 wallNormal = wallInfo.normal;
//    float velocityIntoWall = glm::dot(_velocity, -wallNormal);
//
//    if (velocityIntoWall > 0.1f) {
//        _velocity += wallNormal * velocityIntoWall;
//
//        if (_velocity.y < JUMP_SPEED * 0.5f) {
//            _velocity.y += WALL_SURF_ACCEL * deltaTime;
//        }
//    }
//}

void Player::UpdateTransform() {
    _transform.position = _position;
    _transform.rotation = glm::vec3(_pitch, _yaw, 0.0f);
    _transform.scale = glm::vec3(1.0f);
}

void Player::UpdatePlayerCamera() {
    _camera.SetPosition(_position);
    _camera.SetRotation(glm::vec3(_pitch, _yaw, 0.0f));
}

void Player::UpdateStats(float deltaTime) {
    _stats.sessionTime += deltaTime;

    float currentSpeed = GetHorizontalSpeed();
    _stats.currentSpeedUPS = GetSpeedUPS();

    if (currentSpeed > _stats.maxSpeedReached) {
        _stats.maxSpeedReached = currentSpeed;
    }

    _stats.totalDistance += currentSpeed * deltaTime;
}

void Player::Update(float deltaTime) {
    HandleMouseInput();
    HandleMovementInput();

    glm::vec3 wishdir = GetFront() * _forwardInput + GetRight() * _rightInput;

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

        //if (glm::length(wishdir) > 0.0f) {
        //    WallInfo wallInfo = CheckWallCollision(wishdir);
        //    HandleWallSurfing(wallInfo, deltaTime);
        //}
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
    UpdateTransform();
    UpdatePlayerCamera();
}