#pragma once

#include "common.h"
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

// TODO: Port more of the functions from camera.h to camera.cpp for consistency and speed

class Camera {
private:
    Transform _transform;
    std::string _name;
public:
    Camera() = default;
    Camera(const std::string& name) : _name(name) { 
        std::cout << "Camera created: " << name << std::endl; 
    }
    Camera(const std::string& name, const Transform& transform)
        : _name(name), _transform(transform) {
        std::cout << "Camera created: " << name << " at position ("
            << transform.position.x << ", "
            << transform.position.y << ", "
            << transform.position.z << ")" << std::endl;
    }
    ~Camera() = default;
    Camera(Camera&& other) noexcept
        : _transform(std::move(other._transform)), _name(std::move(other._name)) {
    }

    Camera& operator=(Camera&& other) noexcept {
        if (this != &other) {
            _transform = std::move(other._transform);
            _name = std::move(other._name);
        }
        return *this;
    }

    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;

    const Transform& GetTransform() const { return _transform; }
    void SetTransform(const Transform& transform) { _transform = transform; }

    glm::mat4 GetViewMatrix() const;

    glm::mat4 GetProjectionMatrix(float fov, float aspect, float nearPlane, float farPlane) const;

    const std::string& GetName() const { return _name; }
    void SetName(const std::string& name) { _name = name; }

    void SetPosition(const glm::vec3& position) {
        _transform.position = position;
    }

    void SetRotation(const glm::vec3& rotation) {
        _transform.rotation = rotation;
    }

    void SetScale(const glm::vec3& scale) {
        _transform.scale = scale;
    }
};

class CameraManager {
private:
    std::vector<std::unique_ptr<Camera>> _cameras;
    int _activeCameraIndex;
public:
    CameraManager() : _activeCameraIndex(0) {}

    int AddCamera(const std::string& name) {
        int s = static_cast<int>(_cameras.size());
        _cameras.push_back(std::make_unique<Camera>(name));
        return s;
    }

    int AddCamera(const std::string& name, const Transform& transform) {
		int s = static_cast<int>(_cameras.size());
        _cameras.push_back(std::make_unique<Camera>(name, transform));
        return s;
    }

    int AddExistingCamera(std::unique_ptr<Camera> camera) {
        int s = static_cast<int>(_cameras.size());
        _cameras.push_back(std::move(camera));
        return s;
    }

    int CreateCamera(const std::string& name,
        const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f),
        const glm::vec3& rotation = glm::vec3(0.0f, 0.0f, 0.0f),
        const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f)) {
        Transform transform;
        transform.position = position;
        transform.rotation = rotation;
        transform.scale = scale;

        return AddCamera(name, transform);
    }

    Camera& GetCamera(int index) {
        if (index >= _cameras.size()) {
            throw std::out_of_range("Camera index out of range");
        }
        return *_cameras[index];
    }

    const Camera& GetCamera(int index) const {
        if (index >= _cameras.size()) {
            throw std::out_of_range("Camera index out of range");
        }
        return *_cameras[index];
    }

    Camera& GetActiveCamera() {
        if (_cameras.empty()) {
            throw std::runtime_error("No cameras available");
        }
        return *_cameras[_activeCameraIndex];
    }

    const Camera& GetActiveCamera() const {
        if (_cameras.empty()) {
            throw std::runtime_error("No cameras available");
        }
        return *_cameras[_activeCameraIndex];
    }

    void SetActiveCamera(int index) {
        if (index >= _cameras.size()) {
            throw std::out_of_range("Camera index out of range");
        }
        _activeCameraIndex = index;
    }

    void SetToNextCamera() {
        if (_cameras.empty()) {
            throw std::runtime_error("No cameras available");
        }
		int s = static_cast<int>(_cameras.size());
        _activeCameraIndex = (_activeCameraIndex + 1) % s;
	}

    int FindCameraByName(const std::string& name) const {
        for (int i = 0; i < _cameras.size(); ++i) {
            if (_cameras[i]->GetName() == name) {
                return i;
            }
        }
        throw std::runtime_error("Camera not found: " + name);
    }

    void RemoveCamera(int index) {
        int s = static_cast<int>(_cameras.size());
        if (index >= s) {
            return;
        }

        _cameras.erase(_cameras.begin() + index);

        if (_activeCameraIndex >= s && !_cameras.empty()) {
            _activeCameraIndex = s - 1;
        }
    }

    bool RemoveCameraByName(const std::string& name) {
        int s = static_cast<int>(_cameras.size());
        for (int i = 0; i < s; ++i) {
            if (_cameras[i]->GetName() == name) {
                RemoveCamera(i);
                return true;
            }
        }
        return false;
    }

    int GetCameraCount() const { int s = static_cast<int>(_cameras.size()); return s; }
    bool HasCameras() const { return !_cameras.empty(); }
    int GetActiveCameraIndex() const { return _activeCameraIndex; }

    auto begin() { return _cameras.begin(); }
    auto end() { return _cameras.end(); }
    auto begin() const { return _cameras.begin(); }
    auto end() const { return _cameras.end(); }

    void Clear() {
        _cameras.clear();
        _activeCameraIndex = 0;
    }
};