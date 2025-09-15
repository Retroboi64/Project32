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

#include "camera.h"

glm::mat4 Camera::GetViewMatrix() const {
	float x = cos(glm::radians(_transform.rotation.y)) * cos(glm::radians(_transform.rotation.x));
	float y = sin(glm::radians(_transform.rotation.x));
	float z = sin(glm::radians(_transform.rotation.y)) * cos(glm::radians(_transform.rotation.x));
	glm::vec3 front = glm::normalize(glm::vec3(x, y, z));
	glm::vec3 target = _transform.position + front;
	return glm::lookAt(_transform.position, target, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 Camera::GetProjectionMatrix(float fov, float aspect, float nearPlane, float farPlane) const {
	return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
}
