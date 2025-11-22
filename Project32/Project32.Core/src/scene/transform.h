#pragma once

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "../common.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform {
private:
    glm::vec3 m_position;
    glm::quat m_rotation;
    glm::vec3 m_scale;

    mutable glm::mat4 m_cachedMatrix;
    mutable bool m_matrixDirty = true;

public:
    Transform()
        : m_position(P32::Constants::V3_ZERO)
        , m_rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f))
        , m_scale(P32::Constants::V3_ONE) {
    }

    Transform(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scl)
        : m_position(pos)
        , m_rotation(glm::quat(rot))
        , m_scale(scl) {
    }

    // Fast matrix generation with caching
    inline const glm::mat4& GetMatrix() const {
        if (m_matrixDirty) {
            m_cachedMatrix = glm::translate(glm::mat4(1.0f), m_position);
            m_cachedMatrix *= glm::mat4_cast(m_rotation);
            m_cachedMatrix = glm::scale(m_cachedMatrix, m_scale);
            m_matrixDirty = false;
        }
        return m_cachedMatrix;
    }

    inline glm::vec3 GetForwardVector() const {
        return m_rotation * P32::Constants::V3_FORWARD;
    }

    inline glm::vec3 GetUpVector() const {
        return m_rotation * P32::Constants::V3_UP;
    }

    inline glm::vec3 GetRightVector() const {
        return m_rotation * P32::Constants::V3_RIGHT;
    }

    // Getters
    inline const glm::vec3& GetPosition() const { return m_position; }
	inline const glm::vec3& GetRotation() const { return glm::eulerAngles(m_rotation); }
    inline const glm::vec3& GetScale() const { return m_scale; }

    inline glm::vec3 GetEulerAnglesRadians() const { return glm::eulerAngles(m_rotation); }
    inline glm::vec3 GetEulerAnglesDegrees() const { return glm::degrees(glm::eulerAngles(m_rotation)); }
    inline const glm::quat& GetRotationQuaternion() const { return m_rotation; }

    // Setters
    inline void SetPosition(const glm::vec3& pos) {
        m_position = pos;
        m_matrixDirty = true;
    }

    inline void SetPosition(float x, float y, float z) {
        m_position = glm::vec3(x, y, z);
        m_matrixDirty = true;
	}

	// Single-axis setters for convenience
    inline void SetPositionX(float x) {
        m_position.x = x;
        m_matrixDirty = true;
	}

    inline void SetPositionY(float y) {
        m_position.y = y;
        m_matrixDirty = true;
    }

    inline void SetPositionZ(float z) {
        m_position.z = z;
        m_matrixDirty = true;
	}

    inline void SetRotation(const glm::vec3& euler) {
        m_rotation = glm::quat(euler);
        m_matrixDirty = true;
    }

    inline void SetRotation(const glm::quat& quat) {
        m_rotation = quat;
        m_matrixDirty = true;
    }

	// Single-axis setters for convenience
	//  And look into changing name convention later
	//   To something like SetRotationPitch, etc.
    inline void SetRotationX(float pitch) {
        glm::vec3 euler = glm::eulerAngles(m_rotation);
        euler.x = pitch;
        m_rotation = glm::quat(euler);
        m_matrixDirty = true;
	}

    inline void SetRotationY(float yaw) {
        glm::vec3 euler = glm::eulerAngles(m_rotation);
        euler.y = yaw;
        m_rotation = glm::quat(euler);
        m_matrixDirty = true;
    }

    inline void SetRotationZ(float roll) {
        glm::vec3 euler = glm::eulerAngles(m_rotation);
        euler.z = roll;
        m_rotation = glm::quat(euler);
        m_matrixDirty = true;
	}

    inline void SetScale(const glm::vec3& scl) {
        m_scale = scl;
        m_matrixDirty = true;
    }

    inline void SetScale(float uniformScale) {
        m_scale = glm::vec3(uniformScale);
        m_matrixDirty = true;
    }

    inline glm::vec3 AddPosition(const glm::vec3& delta) {
        m_position += delta;
        m_matrixDirty = true;
        return m_position;
    }

    inline glm::vec3 AddRotation(const glm::vec3& delta) {
        m_rotation = glm::quat(delta) * m_rotation;
        m_matrixDirty = true;
        return glm::eulerAngles(m_rotation);
    }

    inline void Translate(const glm::vec3& delta) {
        m_position += delta;
        m_matrixDirty = true;
    }

    inline void TranslateLocal(const glm::vec3& delta) {
        m_position += m_rotation * delta;
        m_matrixDirty = true;
    }

    inline void Rotate(const glm::vec3& eulerDelta) {
        m_rotation = glm::quat(eulerDelta) * m_rotation;
        m_matrixDirty = true;
    }

    inline void RotateAround(const glm::vec3& point, const glm::vec3& axis, float angle) {
        glm::vec3 direction = m_position - point;
        glm::quat rot = glm::angleAxis(angle, axis);
        m_position = point + (rot * direction);
        m_matrixDirty = true;
    }

    // Utility
    inline void LookAt(const glm::vec3& target, const glm::vec3& up = P32::Constants::V3_UP) {
        glm::vec3 direction = glm::normalize(target - m_position);
        m_rotation = glm::quatLookAt(direction, up);
        m_matrixDirty = true;
    }

    inline void Reset() {
        m_position = P32::Constants::V3_ZERO;
        m_rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        m_scale = P32::Constants::V3_ONE;
        m_matrixDirty = true;
    }
};

#endif // TRANSFORM_H