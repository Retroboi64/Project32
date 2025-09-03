#include "camera.h"

const Transform& Camera::GetTransform() const {
	return _transform;
}

void Camera::SetTransform(const Transform& transform) {
	_transform = transform;
}

glm::mat4 Camera::GetViewMatrix() const {
	glm::vec3 front;
	front.x = cos(glm::radians(_transform.rotation.y)) * cos(glm::radians(_transform.rotation.x));
	front.y = sin(glm::radians(_transform.rotation.x));
	front.z = sin(glm::radians(_transform.rotation.y)) * cos(glm::radians(_transform.rotation.x));
	front = glm::normalize(front);
	glm::vec3 target = _transform.position + front;
	return glm::lookAt(_transform.position, target, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 Camera::GetProjectionMatrix(float fov, float aspect, float nearPlane, float farPlane) const {
	return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
}
