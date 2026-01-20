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

#include "r_common.h"
#include "backend.h"

std::unique_ptr<IGraphicsBackend> GraphicsBackend::s_instance = nullptr;
BackendType GraphicsBackend::s_currentType = BackendType::UNDEFINED;

IGraphicsBackend* GraphicsBackend::Get() {
    if (!s_instance) {
        return nullptr;
    }
    return s_instance.get();
}

BackendType GraphicsBackend::GetCurrentType() {
    return s_currentType;
}

bool GraphicsBackend::Initialize(BackendType type) {
    if (s_instance) {
        return false;
    }

    try {
        switch (type) {
        case BackendType::OPENGL:
            // s_instance = std::make_unique<OpenGLBackend>();
            break;

        case BackendType::VULKAN:
            return false;

        case BackendType::DX11:
            return false;

        case BackendType::DX12:
            return false;

        default:
            return false;
        }

        if (!s_instance->Init()) {
            s_instance.reset();
            return false;
        }

        s_currentType = type;
        return true;
    }
    catch (const std::exception& e) {
        s_instance.reset();
        return false;
    }
}

void GraphicsBackend::Destroy() {
    if (s_instance) {
        s_instance->Shutdown();
        s_instance.reset();
        s_currentType = BackendType::UNDEFINED;
    }
}