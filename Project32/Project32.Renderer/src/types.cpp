#include "r_common.h"

namespace {
    bool g_typesInitialized = false;
}

namespace GraphicsTypes {
    void Initialize() {
        if (g_typesInitialized) {
			spdlog::warn("[GraphicsTypes] Already initialized!");
            return;
        }

        BackendType currentBackend = GraphicsBackend::GetCurrentType();
        if (currentBackend == BackendType::UNDEFINED) {
			spdlog::error("[GraphicsTypes] No graphics backend initialized! Call GraphicsBackend::Initialize() first.");
            return;
        }

		spdlog::info("[GraphicsTypes] Initializing types for backend: {}", static_cast<int>(currentBackend));
        g_typesInitialized = true;
    }

    void Shutdown() {
        if (!g_typesInitialized) {
            return;
        }

		spdlog::info("[GraphicsTypes] Shutting down graphics types");
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
			spdlog::error("[MeshFactory] Vulkan not implemented yet!");
            return nullptr;

        case BackendType::DX11:
			spdlog::error("[MeshFactory] DirectX 11 not implemented yet!");
            return nullptr;

        case BackendType::DX12:
			spdlog::error("[MeshFactory] DirectX 12 not implemented yet!");
            return nullptr;

        default:
			spdlog::error("[MeshFactory] Unknown backend type!");
            return nullptr;
        }
    }

    std::unique_ptr<Mesh> CreateCube() {
        BackendType backend = GraphicsBackend::GetCurrentType();

        switch (backend) {
        case BackendType::OPENGL:
            return StaticMeshes::GetCube();

        case BackendType::VULKAN:
			spdlog::error("[MeshFactory] Vulkan not implemented yet!");
            return nullptr;

        case BackendType::DX11:
			spdlog::error("[MeshFactory] DirectX 11 not implemented yet!");
            return nullptr;

        case BackendType::DX12:
			spdlog::error("[MeshFactory] DirectX 12 not implemented yet!");
            return nullptr;

        default:
			spdlog::error("[MeshFactory] Unknown backend type!");
            return nullptr;
        }
    }

    std::unique_ptr<Mesh> CreateCylinder(unsigned int segments, float height, float radius) {
        BackendType backend = GraphicsBackend::GetCurrentType();

        switch (backend) {
        case BackendType::OPENGL:
            return StaticMeshes::GetCylinder(segments, height, radius);

        case BackendType::VULKAN:
			spdlog::error("[MeshFactory] Vulkan not implemented yet!");
            return nullptr;

        case BackendType::DX11:
			spdlog::error("[MeshFactory] DirectX 11 not implemented yet!");
            return nullptr;

        case BackendType::DX12:
			spdlog::error("[MeshFactory] DirectX 12 not implemented yet!");
            return nullptr;

        default:
			spdlog::error("[MeshFactory] Unknown backend type!");
            return nullptr;
        }
    }

	// TODO: Improve error logging consistency
    std::unique_ptr<Mesh> CreateSphere(unsigned int latitudeSegments, unsigned int longitudeSegments, float radius) {
        BackendType backend = GraphicsBackend::GetCurrentType();

        switch (backend) {
        case BackendType::OPENGL:
            return StaticMeshes::GetSphere(latitudeSegments, longitudeSegments, radius);

        case BackendType::VULKAN:
			spdlog::error("[MeshFactory] Vulkan not implemented yet!");
            return nullptr;

        case BackendType::DX11:
			spdlog::error("[MeshFactory] DirectX 11 not implemented yet!");
            return nullptr;

        case BackendType::DX12:
			spdlog::error("[MeshFactory] DirectX 12 not implemented yet!");
            return nullptr;

        default:
			spdlog::error("[MeshFactory] Unknown backend type!");
            return nullptr;
        }
    }

    std::unique_ptr<Mesh> CreateCapsule(unsigned int segments, unsigned int rings, float height, float radius) {
        BackendType backend = GraphicsBackend::GetCurrentType();

        switch (backend) {
        case BackendType::OPENGL:
            return StaticMeshes::GetCapsule(segments, rings, height, radius);

        case BackendType::VULKAN:
			spdlog::error("[MeshFactory] Vulkan not implemented yet!");
            return nullptr;

        case BackendType::DX11:
            std::cerr << "[MeshFactory] DirectX 11 not implemented yet!" << std::endl;
            return nullptr;

        case BackendType::DX12:
			spdlog::error("[MeshFactory] DirectX 12 not implemented yet!");
            return nullptr;

        default:
			spdlog::error("[MeshFactory] Unknown backend type!");
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
			spdlog::error("[ShaderFactory] Vulkan not implemented yet!");
            return nullptr;

        case BackendType::DX11:
			spdlog::error("[ShaderFactory] DirectX 11 not implemented yet!");
            return nullptr;

        case BackendType::DX12:
			spdlog::error("[ShaderFactory] DirectX 12 not implemented yet!");
            return nullptr;

        default:
			spdlog::error("[ShaderFactory] Unknown backend type!");
            return nullptr;
        }
    }

    std::unique_ptr<ShaderManager> CreateShaderManager() {
        BackendType backend = GraphicsBackend::GetCurrentType();

        switch (backend) {
        case BackendType::OPENGL:
            return std::make_unique<ShaderManager>();

        case BackendType::VULKAN:
			spdlog::error("[ShaderFactory] Vulkan not implemented yet!");
            return nullptr;

        case BackendType::DX11:
			spdlog::error("[ShaderFactory] DirectX 11 not implemented yet!");
            return nullptr;

        case BackendType::DX12:
			spdlog::error("[ShaderFactory] DirectX 12 not implemented yet!");
            return nullptr;

        default:
			spdlog::error("[ShaderFactory] Unknown backend type!");
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
			spdlog::error("[TextureFactory] Vulkan not implemented yet!");
            return nullptr;

        case BackendType::DX11:
			spdlog::error("[TextureFactory] DirectX 11 not implemented yet!");
            return nullptr;

        case BackendType::DX12:
			spdlog::error("[TextureFactory] DirectX 12 not implemented yet!");
            return nullptr;

        default:
			spdlog::error("[TextureFactory] Unknown backend type!");
            return nullptr;
        }
    }

    std::unique_ptr<TextureManager> CreateTextureManager() {
        BackendType backend = GraphicsBackend::GetCurrentType();

        switch (backend) {
        case BackendType::OPENGL:
            return std::make_unique<TextureManager>();

        case BackendType::VULKAN:
			spdlog::error("[TextureFactory] Vulkan not implemented yet!");
            return nullptr;

        case BackendType::DX11:
            std::cerr << "[TextureFactory] DirectX 11 not implemented yet!" << std::endl;
            return nullptr;

        case BackendType::DX12:
			spdlog::error("[TextureFactory] DirectX 12 not implemented yet!");
            return nullptr;

        default:
			spdlog::error("[TextureFactory] Unknown backend type!");
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
			spdlog::error("[SkyboxFactory] Vulkan not implemented yet!");
            return nullptr;

        case BackendType::DX11:
			spdlog::error("[SkyboxFactory] DirectX 11 not implemented yet!");
            return nullptr;

        case BackendType::DX12:
			spdlog::error("[SkyboxFactory] DirectX 12 not implemented yet!");
            return nullptr;

        default:
            std::cerr << "[SkyboxFactory] Unknown backend type!" << std::endl;
            return nullptr;
        }
    }
}