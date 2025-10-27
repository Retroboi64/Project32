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

#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include "../common.h"

enum class CameraProjectionType {
    Perspective,
    Orthographic
};

struct CameraProperties {
    float fov = 90.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    float orthoSize = 10.0f;
    CameraProjectionType projectionType = CameraProjectionType::Perspective;
};

class Camera {
private:
    Transform _transform;
    std::string _name;
    CameraProperties _properties;
    bool _isActive;
    glm::vec3 _up;

public:
    Camera() : _isActive(false), _up(0.0f, 1.0f, 0.0f) {}

    Camera(const std::string& name)
        : _name(name), _isActive(false), _up(0.0f, 1.0f, 0.0f) {
        std::cout << "Camera created: " << name << std::endl;
    }

    Camera(const std::string& name, const Transform& transform)
        : _name(name), _transform(transform), _isActive(false), _up(0.0f, 1.0f, 0.0f) {
        std::cout << "Camera created: " << name << " at position ("
            << transform.position.x << ", "
            << transform.position.y << ", "
            << transform.position.z << ")" << std::endl;
    }

    ~Camera() = default;

    Camera(Camera&& other) noexcept
        : _transform(std::move(other._transform))
        , _name(std::move(other._name))
        , _properties(std::move(other._properties))
        , _isActive(other._isActive)
        , _up(std::move(other._up)) {
    }

    Camera& operator=(Camera&& other) noexcept {
        if (this != &other) {
            _transform = std::move(other._transform);
            _name = std::move(other._name);
            _properties = std::move(other._properties);
            _isActive = other._isActive;
            _up = std::move(other._up);
        }
        return *this;
    }

    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;

    const Transform& GetTransform() const { return _transform; }
    void SetTransform(const Transform& transform) { _transform = transform; }

    const glm::vec3& GetPosition() const { return _transform.position; }
    const glm::vec3& GetRotation() const { return _transform.rotation; }
    const glm::vec3& GetScale() const { return _transform.scale; }

    void SetPosition(const glm::vec3& position) { _transform.position = position; }
    void SetRotation(const glm::vec3& rotation) { _transform.rotation = rotation; }
    void SetScale(const glm::vec3& scale) { _transform.scale = scale; }

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix(float aspect) const;
    glm::mat4 GetProjectionMatrix(float fov, float aspect, float nearPlane, float farPlane) const;

    glm::vec3 GetForward() const;
    glm::vec3 GetRight() const;
    glm::vec3 GetUp() const { return _up; }
    void SetUp(const glm::vec3& up) { _up = glm::normalize(up); }

    void Move(const glm::vec3& offset);
    void MoveForward(float distance);
    void MoveRight(float distance);
    void MoveUp(float distance);

    void Rotate(const glm::vec3& eulerAngles);
    void LookAt(const glm::vec3& target);

    const std::string& GetName() const { return _name; }
    void SetName(const std::string& name) { _name = name; }

    bool IsActive() const { return _isActive; }
    void SetActive(bool active) { _isActive = active; }

    CameraProperties& GetProperties() { return _properties; }
    const CameraProperties& GetProperties() const { return _properties; }
    void SetProperties(const CameraProperties& props) { _properties = props; }

    float GetFOV() const { return _properties.fov; }
    void SetFOV(float fov) { _properties.fov = glm::clamp(fov, 1.0f, 179.0f); }

    float GetNearPlane() const { return _properties.nearPlane; }
    void SetNearPlane(float near) { _properties.nearPlane = (NEAR_PLANE > 0.001f) ? NEAR_PLANE : 0.001f; }

    float GetFarPlane() const { return _properties.farPlane; }
    void SetFarPlane(float far) {
        float minFar = _properties.nearPlane + 0.1f;
        _properties.farPlane = (FAR_PLANE > minFar) ? FAR_PLANE : minFar;
    }

    CameraProjectionType GetProjectionType() const { return _properties.projectionType; }
    void SetProjectionType(CameraProjectionType type) { _properties.projectionType = type; }
};

class CameraManager {
private:
    std::vector<std::unique_ptr<Camera>> _cameras;
    int _activeCameraIndex;

public:
    CameraManager() : _activeCameraIndex(-1) {}

    int AddCamera(const std::string& name);
    int AddCamera(const std::string& name, const Transform& transform);
    int AddExistingCamera(std::unique_ptr<Camera> camera);

    int CreateCamera(const std::string& name,
        const glm::vec3& position = glm::vec3(0.0f),
        const glm::vec3& rotation = glm::vec3(0.0f),
        const glm::vec3& scale = glm::vec3(1.0f));

    Camera* GetCamera(int index);
    const Camera* GetCamera(int index) const;
    Camera* GetCameraByName(const std::string& name);
    const Camera* GetCameraByName(const std::string& name) const;

    Camera* GetActiveCamera();
    const Camera* GetActiveCamera() const;

    void SetActiveCamera(int index);
    bool SetActiveCameraByName(const std::string& name);
    void SetToNextCamera();
    void SetToPreviousCamera();

    int FindCameraByName(const std::string& name) const;
    std::optional<int> TryFindCameraByName(const std::string& name) const;

    void RemoveCamera(int index);
    bool RemoveCameraByName(const std::string& name);

    int GetCameraCount() const { return static_cast<int>(_cameras.size()); }
    bool HasCameras() const { return !_cameras.empty(); }
    bool HasActiveCamera() const { return _activeCameraIndex >= 0 && _activeCameraIndex < GetCameraCount(); }
    int GetActiveCameraIndex() const { return _activeCameraIndex; }

    auto begin() { return _cameras.begin(); }
    auto end() { return _cameras.end(); }
    auto begin() const { return _cameras.begin(); }
    auto end() const { return _cameras.end(); }

    void Clear();

    void PrintCameraInfo() const;
};

#endif // CAMERA_H