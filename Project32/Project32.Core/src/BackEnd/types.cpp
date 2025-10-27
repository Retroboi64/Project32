#include "../common.h"
#include "common.h"
#include "types.h"
#include "backend.h"

namespace {
    bool g_typesInitialized = false;
}

namespace GraphicsTypes {
    void Initialize() {
        if (g_typesInitialized) {
            std::cerr << "[GraphicsTypes] Already initialized!" << std::endl;
            return;
        }

        BackendType currentBackend = GraphicsBackend::GetCurrentType();
        if (currentBackend == BackendType::UNDEFINED) {
            std::cerr << "[GraphicsTypes] No graphics backend initialized! Call GraphicsBackend::Initialize() first." << std::endl;
            return;
        }

        std::cout << "[GraphicsTypes] Initializing types for backend: " << static_cast<int>(currentBackend) << std::endl;
        g_typesInitialized = true;
    }

    void Shutdown() {
        if (!g_typesInitialized) {
            return;
        }

        std::cout << "[GraphicsTypes] Shutting down graphics types" << std::endl;
        g_typesInitialized = false;
    }

    bool IsInitialized() {
        return g_typesInitialized;
    }
}

namespace MeshFactory {
    std::unique_ptr<Mesh> CreateQuad() {
        BackendType backend = GraphicsBackend::GetCurrentType();

        switch (backend) {
        case BackendType::OPENGL:
            return StaticMeshes::GetQuad();

        case BackendType::VULKAN:
            std::cerr << "[MeshFactory] Vulkan not implemented yet!" << std::endl;
            return nullptr;

        case BackendType::DX11:
            std::cerr << "[MeshFactory] DirectX 11 not implemented yet!" << std::endl;
            return nullptr;

        case BackendType::DX12:
            std::cerr << "[MeshFactory] DirectX 12 not implemented yet!" << std::endl;
            return nullptr;

        default:
            std::cerr << "[MeshFactory] Unknown backend type!" << std::endl;
            return nullptr;
        }
    }

    std::unique_ptr<Mesh> CreateCube() {
        BackendType backend = GraphicsBackend::GetCurrentType();

        switch (backend) {
        case BackendType::OPENGL:
            return StaticMeshes::GetCube();

        case BackendType::VULKAN:
            std::cerr << "[MeshFactory] Vulkan not implemented yet!" << std::endl;
            return nullptr;

        case BackendType::DX11:
            std::cerr << "[MeshFactory] DirectX 11 not implemented yet!" << std::endl;
            return nullptr;

        case BackendType::DX12:
            std::cerr << "[MeshFactory] DirectX 12 not implemented yet!" << std::endl;
            return nullptr;

        default:
            std::cerr << "[MeshFactory] Unknown backend type!" << std::endl;
            return nullptr;
        }
    }

    std::unique_ptr<Mesh> CreateCylinder(unsigned int segments, float height, float radius) {
        BackendType backend = GraphicsBackend::GetCurrentType();

        switch (backend) {
        case BackendType::OPENGL:
            return StaticMeshes::GetCylinder(segments, height, radius);

        case BackendType::VULKAN:
            std::cerr << "[MeshFactory] Vulkan not implemented yet!" << std::endl;
            return nullptr;

        case BackendType::DX11:
            std::cerr << "[MeshFactory] DirectX 11 not implemented yet!" << std::endl;
            return nullptr;

        case BackendType::DX12:
            std::cerr << "[MeshFactory] DirectX 12 not implemented yet!" << std::endl;
            return nullptr;

        default:
            std::cerr << "[MeshFactory] Unknown backend type!" << std::endl;
            return nullptr;
        }
    }

    std::unique_ptr<Mesh> CreateSphere(unsigned int latitudeSegments, unsigned int longitudeSegments, float radius) {
        BackendType backend = GraphicsBackend::GetCurrentType();

        switch (backend) {
        case BackendType::OPENGL:
            return StaticMeshes::GetSphere(latitudeSegments, longitudeSegments, radius);

        case BackendType::VULKAN:
            std::cerr << "[MeshFactory] Vulkan not implemented yet!" << std::endl;
            return nullptr;

        case BackendType::DX11:
            std::cerr << "[MeshFactory] DirectX 11 not implemented yet!" << std::endl;
            return nullptr;

        case BackendType::DX12:
            std::cerr << "[MeshFactory] DirectX 12 not implemented yet!" << std::endl;
            return nullptr;

        default:
            std::cerr << "[MeshFactory] Unknown backend type!" << std::endl;
            return nullptr;
        }
    }

    std::unique_ptr<Mesh> CreateCapsule(unsigned int segments, unsigned int rings, float height, float radius) {
        BackendType backend = GraphicsBackend::GetCurrentType();

        switch (backend) {
        case BackendType::OPENGL:
            return StaticMeshes::GetCapsule(segments, rings, height, radius);

        case BackendType::VULKAN:
            std::cerr << "[MeshFactory] Vulkan not implemented yet!" << std::endl;
            return nullptr;

        case BackendType::DX11:
            std::cerr << "[MeshFactory] DirectX 11 not implemented yet!" << std::endl;
            return nullptr;

        case BackendType::DX12:
            std::cerr << "[MeshFactory] DirectX 12 not implemented yet!" << std::endl;
            return nullptr;

        default:
            std::cerr << "[MeshFactory] Unknown backend type!" << std::endl;
            return nullptr;
        }
    }
}

// ==========================================
// Shader Factory Implementation
// ==========================================
namespace ShaderFactory {
    std::unique_ptr<Shader> CreateShader() {
        BackendType backend = GraphicsBackend::GetCurrentType();

        switch (backend) {
        case BackendType::OPENGL:
            return std::make_unique<Shader>();

        case BackendType::VULKAN:
            std::cerr << "[ShaderFactory] Vulkan not implemented yet!" << std::endl;
            return nullptr;

        case BackendType::DX11:
            std::cerr << "[ShaderFactory] DirectX 11 not implemented yet!" << std::endl;
            return nullptr;

        case BackendType::DX12:
            std::cerr << "[ShaderFactory] DirectX 12 not implemented yet!" << std::endl;
            return nullptr;

        default:
            std::cerr << "[ShaderFactory] Unknown backend type!" << std::endl;
            return nullptr;
        }
    }

    std::unique_ptr<ShaderManager> CreateShaderManager() {
        BackendType backend = GraphicsBackend::GetCurrentType();

        switch (backend) {
        case BackendType::OPENGL:
            return std::make_unique<ShaderManager>();

        case BackendType::VULKAN:
            std::cerr << "[ShaderFactory] Vulkan not implemented yet!" << std::endl;
            return nullptr;

        case BackendType::DX11:
            std::cerr << "[ShaderFactory] DirectX 11 not implemented yet!" << std::endl;
            return nullptr;

        case BackendType::DX12:
            std::cerr << "[ShaderFactory] DirectX 12 not implemented yet!" << std::endl;
            return nullptr;

        default:
            std::cerr << "[ShaderFactory] Unknown backend type!" << std::endl;
            return nullptr;
        }
    }
}

namespace TextureFactory {
    std::unique_ptr<Texture> CreateTexture(const std::string& name) {
        BackendType backend = GraphicsBackend::GetCurrentType();

        switch (backend) {
        case BackendType::OPENGL:
            return std::make_unique<Texture>(name);

        case BackendType::VULKAN:
            std::cerr << "[TextureFactory] Vulkan not implemented yet!" << std::endl;
            return nullptr;

        case BackendType::DX11:
            std::cerr << "[TextureFactory] DirectX 11 not implemented yet!" << std::endl;
            return nullptr;

        case BackendType::DX12:
            std::cerr << "[TextureFactory] DirectX 12 not implemented yet!" << std::endl;
            return nullptr;

        default:
            std::cerr << "[TextureFactory] Unknown backend type!" << std::endl;
            return nullptr;
        }
    }

    std::unique_ptr<TextureManager> CreateTextureManager() {
        BackendType backend = GraphicsBackend::GetCurrentType();

        switch (backend) {
        case BackendType::OPENGL:
            return std::make_unique<TextureManager>();

        case BackendType::VULKAN:
            std::cerr << "[TextureFactory] Vulkan not implemented yet!" << std::endl;
            return nullptr;

        case BackendType::DX11:
            std::cerr << "[TextureFactory] DirectX 11 not implemented yet!" << std::endl;
            return nullptr;

        case BackendType::DX12:
            std::cerr << "[TextureFactory] DirectX 12 not implemented yet!" << std::endl;
            return nullptr;

        default:
            std::cerr << "[TextureFactory] Unknown backend type!" << std::endl;
            return nullptr;
        }
    }
}

namespace SkyboxFactory {
    std::unique_ptr<Skybox> CreateSkybox() {
        BackendType backend = GraphicsBackend::GetCurrentType();

        switch (backend) {
        case BackendType::OPENGL:
            return std::make_unique<Skybox>();

        case BackendType::VULKAN:
            std::cerr << "[SkyboxFactory] Vulkan not implemented yet!" << std::endl;
            return nullptr;

        case BackendType::DX11:
            std::cerr << "[SkyboxFactory] DirectX 11 not implemented yet!" << std::endl;
            return nullptr;

        case BackendType::DX12:
            std::cerr << "[SkyboxFactory] DirectX 12 not implemented yet!" << std::endl;
            return nullptr;

        default:
            std::cerr << "[SkyboxFactory] Unknown backend type!" << std::endl;
            return nullptr;
        }
    }
}