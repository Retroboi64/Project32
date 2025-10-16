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
#include "backend.h"
#include "OpenGL/GL_backend.h"

 // Static member initialization
std::unique_ptr<IGraphicsBackend> GraphicsBackend::s_backend = nullptr;
BackendType GraphicsBackend::s_currentType = BackendType::UNDEFINED;

// BackendFactory implementation
std::unique_ptr<IGraphicsBackend> BackendFactory::Create(BackendType type) {
    switch (type) {
    case BackendType::OPENGL:
        return std::make_unique<OpenGLBackend>();

    case BackendType::VULKAN:
        std::cerr << "Vulkan backend not yet implemented" << std::endl;
        return nullptr;

    case BackendType::DX11:
        std::cerr << "DirectX 11 backend not yet implemented" << std::endl;
        return nullptr;

    case BackendType::DX12:
        std::cerr << "DirectX 12 backend not yet implemented" << std::endl;
        return nullptr;

    default:
        std::cerr << "Unknown backend type" << std::endl;
        return nullptr;
    }
}

// GraphicsBackend implementation
void GraphicsBackend::Initialize(BackendType type) {
    if (s_backend) {
        std::cerr << "Graphics backend already initialized. Shutting down previous backend." << std::endl;
        Shutdown();
    }

    s_backend = BackendFactory::Create(type);
    if (!s_backend) {
        throw std::runtime_error("Failed to create graphics backend");
    }

    if (!s_backend->Init()) {
        s_backend.reset();
        throw std::runtime_error("Failed to initialize graphics backend");
    }

    s_currentType = type;
    std::cout << "Graphics backend initialized: " << s_backend->GetRendererName() << std::endl;
}

void GraphicsBackend::Shutdown() {
    if (s_backend) {
        s_backend->Shutdown();
        s_backend.reset();
        s_currentType = BackendType::UNDEFINED;
        std::cout << "Graphics backend shut down" << std::endl;
    }
}

IGraphicsBackend* GraphicsBackend::Get() {
    if (!s_backend) {
        std::cerr << "ERROR: Graphics backend not initialized! Call GraphicsBackend::Initialize() first." << std::endl;
        return nullptr;
    }
    return s_backend.get();
}

BackendType GraphicsBackend::GetCurrentType() {
    return s_currentType;
}