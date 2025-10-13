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

#include "glm/glm.hpp"

// TODO: Add more math utilities as needed
namespace Math {
    inline float Lerp(float a, float b, float t) {
        return a + t * (b - a);
    }

    inline glm::vec3 Lerp(const glm::vec3& a, const glm::vec3& b, float t) {
        return a + t * (b - a);
    }

    inline glm::vec3 GetForward(float pitchDeg, float yawDeg) {
        float pitch = glm::radians(pitchDeg);
        float yaw = glm::radians(yawDeg);

        float cp = cos(pitch);

        glm::vec3 forward(cp * cos(yaw),    
            sin(pitch),       
            cp * sin(yaw));

        return glm::normalize(forward);
    }

    inline glm::mat4 CreateViewMatrix(const glm::vec3& position, const glm::vec3& forward, const glm::vec3& up) {
        glm::vec3 f = glm::normalize(forward);              
        glm::vec3 r = glm::normalize(glm::cross(f, up));    
        glm::vec3 u = glm::cross(r, f);                     

        glm::mat4 view(1.0f);
        view[0] = glm::vec4(r, 0.0f);
        view[1] = glm::vec4(u, 0.0f);
        view[2] = glm::vec4(-f, 0.0f);
        view[3] = glm::vec4(
            -glm::dot(r, position),
            -glm::dot(u, position),
            glm::dot(f, position),
            1.0f
        );

        return view;
    }


    inline glm::vec3 Reflect(const glm::vec3& incident, const glm::vec3& normal) {
        return incident - 2.0f * glm::dot(incident, normal) * normal;
	}

}