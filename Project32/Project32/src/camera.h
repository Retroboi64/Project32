#pragma once

#include "common.h"

class Camera {
private:
	Transform _transform;
public:
	Camera() = default;
	~Camera() = default;
	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;
	const Transform& GetTransform() const;
	void SetTransform(const Transform& transform);
	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetProjectionMatrix(float fov, float aspect, float nearPlane, float farPlane) const;
};