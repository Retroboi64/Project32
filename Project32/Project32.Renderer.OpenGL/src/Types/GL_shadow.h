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

#ifndef GL_SHADOW_H
#define GL_SHADOW_H 

#include "../GL_common.h"

class ShadowMap {
private:
    unsigned int _depthMapFBO = 0;
    unsigned int _depthMap = 0;
    unsigned int _shadowWidth = 2048;
    unsigned int _shadowHeight = 2048;

    glm::mat4 _lightSpaceMatrix;
    glm::vec3 _lightPos;

public:
    ShadowMap(unsigned int width = 2048, unsigned int height = 2048);
    ~ShadowMap();

    bool Initialize();
    void Cleanup();

    void BeginShadowPass();
    void EndShadowPass();

    unsigned int GetDepthMap() const { return _depthMap; }
    glm::mat4 GetLightSpaceMatrix() const { return _lightSpaceMatrix; }

    void UpdateLightSpaceMatrix(const glm::vec3& lightPos, const glm::vec3& lookAt = glm::vec3(0.0f));

    void BindForReading(int textureUnit = 1);
};

#endif // GL_SHADOW_H