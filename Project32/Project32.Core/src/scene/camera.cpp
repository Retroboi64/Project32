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

#include "../common.h"
#include "camera.h"

glm::mat4 Camera::GetViewMatrix() const {
    glm::vec3 f = GetForward();              
    glm::vec3 r = glm::normalize(glm::cross(f, _up)); 
    glm::vec3 u = glm::cross(r, f);          

    glm::mat4 view(1.0f);

    view[0] = glm::vec4(r, 0.0f);
    view[1] = glm::vec4(u, 0.0f);
    view[2] = glm::vec4(-f, 0.0f);
    view[3] = glm::vec4(
        -glm::dot(r, _transform.GetPosition()),
        -glm::dot(u, _transform.GetPosition()),
        glm::dot(f, _transform.GetPosition()),
        1.0f
    );

    return view;
}

glm::mat4 Camera::GetProjectionMatrix(float aspect) const {
    return GetProjectionMatrix(_properties.fov, aspect, _properties.nearPlane, _properties.farPlane);
}

glm::mat4 Camera::GetProjectionMatrix(float fov, float aspect, float nearPlane, float farPlane) const {
    if (_properties.projectionType == CameraProjectionType::Perspective) {
        return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
    }
    else {
        float halfWidth = _properties.orthoSize * aspect * 0.5f;
        float halfHeight = _properties.orthoSize * 0.5f;
        return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, nearPlane, farPlane);
    }
}

glm::vec3 Camera::GetForward() const {
    return Math::GetForward(_transform.GetRotation().x, _transform.GetRotation().y);;
}

glm::vec3 Camera::GetRight() const {
    return glm::normalize(glm::cross(GetForward(), _up));
}

void Camera::Move(const glm::vec3& offset) {
	_transform.AddPosition(offset);
}

void Camera::MoveForward(float distance) {
	_transform.AddPosition(GetForward() * distance);
}

void Camera::MoveRight(float distance) {
	_transform.AddPosition(GetRight() * distance);
}

void Camera::MoveUp(float distance) {
	_transform.AddPosition(_up * distance);
}

// This Looks a bit messy but it works fine
void Camera::Rotate(const glm::vec3& eulerAngles) {
    _transform.AddRotation(eulerAngles);
    glm::vec3 rot = _transform.GetRotation();
    rot.x = glm::clamp(rot.x, -89.0f, 89.0f);
    rot.y = fmodf(rot.y, 360.0f);
    if (rot.y < 0.0f) rot.y += 360.0f;
    _transform.SetRotation(rot);
}

void Camera::LookAt(const glm::vec3& target) {
    glm::vec3 direction = glm::normalize(target - _transform.GetPosition());

	_transform.SetPositionX(glm::degrees(asin(direction.y)));
	_transform.SetPositionY(glm::degrees(atan2(direction.z, direction.x)));
}

int CameraManager::AddCamera(const std::string& name) {
    int index = static_cast<int>(_cameras.size());
    auto camera = std::make_unique<Camera>(name);

    if (_cameras.empty()) {
        camera->SetActive(true);
        _activeCameraIndex = 0;
    }

    _cameras.push_back(std::move(camera));
    return index;
}

int CameraManager::AddCamera(const std::string& name, const Transform& transform) {
    int index = static_cast<int>(_cameras.size());
    auto camera = std::make_unique<Camera>(name, transform);

    if (_cameras.empty()) {
        camera->SetActive(true);
        _activeCameraIndex = 0;
    }

    _cameras.push_back(std::move(camera));
    return index;
}

int CameraManager::AddExistingCamera(std::unique_ptr<Camera> camera) {
    if (!camera) {
        throw std::invalid_argument("Cannot add null camera");
    }

    int index = static_cast<int>(_cameras.size());

    if (_cameras.empty()) {
        camera->SetActive(true);
        _activeCameraIndex = 0;
    }

    _cameras.push_back(std::move(camera));
    return index;
}

int CameraManager::CreateCamera(const std::string& name,
    const glm::vec3& position,
    const glm::vec3& rotation,
    const glm::vec3& scale) {

	// TODO: Look at this later
    Transform transform{
        position,
        rotation,
        scale,
    };

    return AddCamera(name, transform);
}

Camera* CameraManager::GetCamera(int index) {
    if (index < 0 || index >= static_cast<int>(_cameras.size())) {
        return nullptr;
    }
    return _cameras[index].get();
}

const Camera* CameraManager::GetCamera(int index) const {
    if (index < 0 || index >= static_cast<int>(_cameras.size())) {
        return nullptr;
    }
    return _cameras[index].get();
}

Camera* CameraManager::GetCameraByName(const std::string& name) {
    for (auto& camera : _cameras) {
        if (camera->GetName() == name) {
            return camera.get();
        }
    }
    return nullptr;
}

const Camera* CameraManager::GetCameraByName(const std::string& name) const {
    for (const auto& camera : _cameras) {
        if (camera->GetName() == name) {
            return camera.get();
        }
    }
    return nullptr;
}

Camera* CameraManager::GetActiveCamera() {
    if (!HasActiveCamera()) {
        return nullptr;
    }
    return _cameras[_activeCameraIndex].get();
}

const Camera* CameraManager::GetActiveCamera() const {
    if (!HasActiveCamera()) {
        return nullptr;
    }
    return _cameras[_activeCameraIndex].get();
}

void CameraManager::SetActiveCamera(int index) {
    if (index < 0 || index >= static_cast<int>(_cameras.size())) {
        std::cerr << "[CameraManager] Invalid camera index: " << index << std::endl;
        return;
    }

    if (HasActiveCamera()) {
        _cameras[_activeCameraIndex]->SetActive(false);
    }

    _activeCameraIndex = index;
    _cameras[_activeCameraIndex]->SetActive(true);

    std::cout << "[CameraManager] Switched to camera: "
        << _cameras[_activeCameraIndex]->GetName() << std::endl;
}

bool CameraManager::SetActiveCameraByName(const std::string& name) {
    auto index = TryFindCameraByName(name);
    if (index.has_value()) {
        SetActiveCamera(index.value());
        return true;
    }
    return false;
}

void CameraManager::SetToNextCamera() {
    if (_cameras.empty()) {
        return;
    }

    int nextIndex = (_activeCameraIndex + 1) % static_cast<int>(_cameras.size());
    SetActiveCamera(nextIndex);
}

void CameraManager::SetToPreviousCamera() {
    if (_cameras.empty()) {
        return;
    }

    int prevIndex = _activeCameraIndex - 1;
    if (prevIndex < 0) {
        prevIndex = static_cast<int>(_cameras.size()) - 1;
    }
    SetActiveCamera(prevIndex);
}

int CameraManager::FindCameraByName(const std::string& name) const {
    for (int i = 0; i < static_cast<int>(_cameras.size()); ++i) {
        if (_cameras[i]->GetName() == name) {
            return i;
        }
    }
    throw std::runtime_error("Camera not found: " + name);
}

std::optional<int> CameraManager::TryFindCameraByName(const std::string& name) const {
    for (int i = 0; i < static_cast<int>(_cameras.size()); ++i) {
        if (_cameras[i]->GetName() == name) {
            return i;
        }
    }
    return std::nullopt;
}

void CameraManager::RemoveCamera(int index) {
    if (index < 0 || index >= static_cast<int>(_cameras.size())) {
        return;
    }

    std::string removedName = _cameras[index]->GetName();
    _cameras.erase(_cameras.begin() + index);

    if (_cameras.empty()) {
        _activeCameraIndex = -1;
    }
    else if (_activeCameraIndex >= static_cast<int>(_cameras.size())) {
        _activeCameraIndex = static_cast<int>(_cameras.size()) - 1;
        _cameras[_activeCameraIndex]->SetActive(true);
    }
    else if (_activeCameraIndex == index) {
        int newIndex = index;
        if (newIndex >= static_cast<int>(_cameras.size())) {
            newIndex = static_cast<int>(_cameras.size()) - 1;
        }
        _activeCameraIndex = newIndex;
        _cameras[_activeCameraIndex]->SetActive(true);
    }

    std::cout << "[CameraManager] Removed camera: " << removedName << std::endl;
}

bool CameraManager::RemoveCameraByName(const std::string& name) {
    auto index = TryFindCameraByName(name);
    if (index.has_value()) {
        RemoveCamera(index.value());
        return true;
    }
    return false;
}

void CameraManager::Clear() {
    _cameras.clear();
    _activeCameraIndex = -1;
    std::cout << "[CameraManager] All cameras cleared" << std::endl;
}

void CameraManager::PrintCameraInfo() const {
    std::cout << "\n=== Camera Manager Info ===" << std::endl;
    std::cout << "Total cameras: " << _cameras.size() << std::endl;
    std::cout << "Active camera index: " << _activeCameraIndex << std::endl;

    for (int i = 0; i < static_cast<int>(_cameras.size()); ++i) {
        const auto& cam = _cameras[i];
        std::cout << "\n[" << i << "] " << cam->GetName();
        if (cam->IsActive()) {
            std::cout << " (ACTIVE)";
        }
        std::cout << std::endl;

        const auto& pos = cam->GetPosition();
        const auto& rot = cam->GetRotation();
        std::cout << "  Position: (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
        std::cout << "  Rotation: (" << rot.x << ", " << rot.y << ", " << rot.z << ")" << std::endl;
        std::cout << "  FOV: " << cam->GetFOV() << std::endl;
        std::cout << "  Projection: "
            << (cam->GetProjectionType() == CameraProjectionType::Perspective ? "Perspective" : "Orthographic")
            << std::endl;
    }
    std::cout << "========================\n" << std::endl;
}