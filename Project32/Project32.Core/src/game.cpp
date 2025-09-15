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

#include "common.h"
#include "game.h"
#include "input.h"
#include "renderer.h"
#include "camera.h"
#include "player.h"
#include "GL.h"

namespace Game {
    Player player;
    CameraManager manager;

    int cam1;
    int cam2;

    static void UpdateCamera() {
        auto& camera = manager.GetCamera(cam1);
		camera.SetTransform(player.GetTransform()); 
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
        std::cout << "V: Switch camera" << std::endl;
        std::cout << "ESC: Exit" << std::endl;
        std::cout << "===============================" << std::endl;

		cam1 = manager.AddExistingCamera(player.GetCamera());
        cam2 = manager.AddCamera("Secondary", Transform{
            .position = glm::vec3(0.0f, 1.0f, 20.0f),
            .rotation = glm::vec3(0.0f, -90.0f, 0.0f),
            .scale = glm::vec3(1.0f)
            });
        manager.SetActiveCamera(cam1);
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
            manager.SetToNextCamera();
        }

        player.Update(deltaTime);
        UpdateCamera();
    }

    glm::mat4 GetViewMatrix() {
        return manager.GetActiveCamera().GetViewMatrix();
    }

    glm::vec3 GetPosition() { return player.GetPosition(); }
	glm::vec3 GetRotation() { return player.GetRotation(); }
    glm::vec3 GetVelocity() { return player.GetVelocity(); }
    float GetSpeed() { return player.GetHorizontalSpeed(); }
    float GetYaw() { return player.GetYaw(); }
    float GetPitch() { return player.GetPitch(); }
    bool IsOnGround() { return player.IsOnGround(); }

    const Player& GetPlayer() { return player; }
}