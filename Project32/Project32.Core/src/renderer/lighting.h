// lighting.h - Enhanced lighting system

#ifndef LIGHTING_H
#define LIGHTING_H

#include "../common.h"

enum class LightType {
    DIRECTIONAL,
    POINT,
    SPOT
};

struct Light {
    LightType type = LightType::POINT;
    glm::vec3 position{ 0.0f, 5.0f, 0.0f };
    glm::vec3 direction{ 0.0f, -1.0f, 0.0f };
    glm::vec3 color{ 1.0f, 1.0f, 1.0f };
    float intensity = 1.0f;
    float range = 50.0f;

    float innerCutoff = glm::cos(glm::radians(12.5f));
    float outerCutoff = glm::cos(glm::radians(17.5f));

    bool castsShadows = false;
    bool enabled = true;

    Light() = default;

    Light(LightType t, const glm::vec3& pos, const glm::vec3& col, float inten = 1.0f)
        : type(t), position(pos), color(col), intensity(inten) {
    }
};

class LightingSystem {
public:
    static constexpr int MAX_LIGHTS = 8;

    LightingSystem() = default;

    int AddLight(const Light& light) {
        if (m_lights.size() >= MAX_LIGHTS) {
            spdlog::warn("[LightingSystem] Maximum light count reached");
            return -1;
        }
        m_lights.push_back(light);
        return static_cast<int>(m_lights.size() - 1);
    }

    void RemoveLight(int index) {
        if (index >= 0 && index < static_cast<int>(m_lights.size())) {
            m_lights.erase(m_lights.begin() + index);
        }
    }

    Light* GetLight(int index) {
        if (index >= 0 && index < static_cast<int>(m_lights.size())) {
            return &m_lights[index];
        }
        return nullptr;
    }

    const std::vector<Light>& GetLights() const {
        return m_lights;
    }

    int GetActiveLightCount() const {
        int count = 0;
        for (const auto& light : m_lights) {
            if (light.enabled) count++;
        }
        return count;
    }

    void Clear() {
        m_lights.clear();
    }

    void BindToShader(IGraphicsBackend* backend, int shaderID) {
        if (!backend) return;

        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> colors;
        std::vector<glm::vec3> directions;
        std::vector<float> ranges;
        std::vector<int> types;

        for (const auto& light : m_lights) {
            if (!light.enabled) continue;

            positions.push_back(light.position);
            colors.push_back(light.color * light.intensity);
            directions.push_back(light.direction);
            ranges.push_back(light.range);
            types.push_back(static_cast<int>(light.type));
        }

        int activeCount = static_cast<int>(positions.size());
        backend->SetShaderInt(shaderID, "numLights", activeCount);

        for (int i = 0; i < activeCount; ++i) {
            std::string base = "lightPositions[" + std::to_string(i) + "]";
            backend->SetShaderVec3(shaderID, base, positions[i]);

            base = "lightColors[" + std::to_string(i) + "]";
            backend->SetShaderVec3(shaderID, base, colors[i]);

            base = "lightDirections[" + std::to_string(i) + "]";
            backend->SetShaderVec3(shaderID, base, directions[i]);

            base = "lightRanges[" + std::to_string(i) + "]";
            backend->SetShaderFloat(shaderID, base, ranges[i]);

            base = "lightTypes[" + std::to_string(i) + "]";
            backend->SetShaderInt(shaderID, base, types[i]);
        }
    }

    void SetupDefaultLighting() {
        Clear();

        Light sun;
        sun.type = LightType::DIRECTIONAL;
        sun.position = glm::vec3(5.0f, 10.0f, 5.0f);
        sun.direction = glm::normalize(glm::vec3(-0.5f, -1.0f, -0.5f));
        sun.color = glm::vec3(1.0f, 0.95f, 0.9f);
        sun.intensity = 1.5f;
        sun.castsShadows = true;
        AddLight(sun);

        Light fill;
        fill.type = LightType::POINT;
        fill.position = glm::vec3(-3.0f, 5.0f, -3.0f);
        fill.color = glm::vec3(0.4f, 0.5f, 0.6f);
        fill.intensity = 0.5f;
        fill.range = 20.0f;
        AddLight(fill);

        spdlog::info("[LightingSystem] Default lighting setup created");
    }

private:
    std::vector<Light> m_lights;
};

#endif // LIGHTING_H