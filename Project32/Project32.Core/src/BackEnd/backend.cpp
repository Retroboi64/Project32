#include "../common.h"
#include "OpenGL/GL_backEnd.h"
#include "backend.h"

/*
  Renderer.dll          ← main interface / manager
  RendererAPI/
    RendererGL.dll        ← OpenGL backend
    RendererVK.dll        ← Vulkan backend
    RendererDX12.dll      ← optional future backend
*/

std::unique_ptr<IGraphicsBackend> GraphicsBackend::s_instance = nullptr;
BackendType GraphicsBackend::s_currentType = BackendType::UNDEFINED;

IGraphicsBackend* GraphicsBackend::Get() {
    if (!s_instance) {
        std::cerr << "[GraphicsBackend] No backend initialized! Call Initialize() first." << std::endl;
        return nullptr;
    }
    return s_instance.get();
}

BackendType GraphicsBackend::GetCurrentType() {
    return s_currentType;
}

bool GraphicsBackend::Initialize(BackendType type) {
    if (s_instance) {
        spdlog::warn("[GraphicsBackend] Backend already initialized!");
        return false;
    }

    try {
        switch (type) {
        case BackendType::OPENGL:
            s_instance = std::make_unique<OpenGLBackend>();
            break;

        case BackendType::VULKAN:
            spdlog::warn("[GraphicsBackend] Vulkan not implemented yet!");
            return false;

        case BackendType::DX11:
            spdlog::warn("[GraphicsBackend] DirectX 11 not implemented yet!");
            return false;

        case BackendType::DX12:
            spdlog::warn("[GraphicsBackend] DirectX 12 not implemented yet!");
            return false;

        default:
            spdlog::warn("[GraphicsBackend] Unknown backend type!");
            return false;
        }

        if (!s_instance->Init()) {
            spdlog::warn("[GraphicsBackend] Failed to initialize backend!");
            s_instance.reset();
            return false;
        }

        s_currentType = type;
        spdlog::info("[GraphicsBackend] Successfully initialized: {}", s_instance->GetRendererName());
        return true;
    }
    catch (const std::exception& e) {
        spdlog::error("[GraphicsBackend] Exception during initialization: {}", e.what());
        s_instance.reset();
        return false;
    }
}

void GraphicsBackend::Destroy() {
    if (s_instance) {
        s_instance->Shutdown();
        s_instance.reset();
        s_currentType = BackendType::UNDEFINED;
        spdlog::info("[GraphicsBackend] Backend destroyed");
    }
}