#include "../common.h"
#include "OpenGL/GL_backEnd.h"
#include "backend.h"

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
        std::cerr << "[GraphicsBackend] Backend already initialized!" << std::endl;
        return false;
    }

    try {
        switch (type) {
        case BackendType::OPENGL:
            s_instance = std::make_unique<OpenGLBackend>();
            break;

        case BackendType::VULKAN:
            std::cerr << "[GraphicsBackend] Vulkan not implemented yet!" << std::endl;
            return false;

        case BackendType::DX11:
            std::cerr << "[GraphicsBackend] DirectX 11 not implemented yet!" << std::endl;
            return false;

        case BackendType::DX12:
            std::cerr << "[GraphicsBackend] DirectX 12 not implemented yet!" << std::endl;
            return false;

        default:
            std::cerr << "[GraphicsBackend] Unknown backend type!" << std::endl;
            return false;
        }

        if (!s_instance->Init()) {
            std::cerr << "[GraphicsBackend] Failed to initialize backend!" << std::endl;
            s_instance.reset();
            return false;
        }

        s_currentType = type;
        std::cout << "[GraphicsBackend] Successfully initialized: "
            << s_instance->GetRendererName() << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "[GraphicsBackend] Exception during initialization: " << e.what() << std::endl;
        s_instance.reset();
        return false;
    }
}

void GraphicsBackend::Destroy() {
    if (s_instance) {
        s_instance->Shutdown();
        s_instance.reset();
        s_currentType = BackendType::UNDEFINED;
        std::cout << "[GraphicsBackend] Backend destroyed" << std::endl;
    }
}