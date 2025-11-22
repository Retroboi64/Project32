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

#include "../../../common.h"
#include "GL_shadow.h"

ShadowMap::ShadowMap(unsigned int width, unsigned int height)
    : _shadowWidth(width), _shadowHeight(height),
      _lightSpaceMatrix(1.0f),
      _lightPos(0.0f, 0.0f, 0.0f)
{
}

ShadowMap::~ShadowMap() {
    Cleanup();
}

bool ShadowMap::Initialize() {
    glGenFramebuffers(1, &_depthMapFBO);

    glGenTextures(1, &_depthMap);
    glBindTexture(GL_TEXTURE_2D, _depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _shadowWidth, _shadowHeight,
        0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthMap, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        spdlog::error("[ShadowMap] Framebuffer is not complete!");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    spdlog::info("[ShadowMap] Initialized {}x{}", _shadowWidth, _shadowHeight);
    return true;
}

void ShadowMap::Cleanup() {
    if (_depthMap != 0) {
        glDeleteTextures(1, &_depthMap);
        _depthMap = 0;
    }
    if (_depthMapFBO != 0) {
        glDeleteFramebuffers(1, &_depthMapFBO);
        _depthMapFBO = 0;
    }
}

void ShadowMap::BeginShadowPass() {
    glViewport(0, 0, _shadowWidth, _shadowHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    glCullFace(GL_FRONT);
}

void ShadowMap::EndShadowPass() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCullFace(GL_BACK);
}

void ShadowMap::UpdateLightSpaceMatrix(const glm::vec3& lightPos, const glm::vec3& lookAt) {
    _lightPos = lightPos;

    float orthoSize = 20.0f;
    glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, 1.0f, 50.0f);

    glm::mat4 lightView = glm::lookAt(lightPos, lookAt, glm::vec3(0.0f, 1.0f, 0.0f));

    _lightSpaceMatrix = lightProjection * lightView;
}

void ShadowMap::BindForReading(int textureUnit) {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, _depthMap);
}