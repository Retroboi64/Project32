#pragma once

namespace Util {
    inline std::string ReadTextFromFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + path);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    // **MOVE THIS**
    inline float Lerp(float a, float b, float t) {
        return a + t * (b - a);
    }

    // **MOVE THIS**
    inline glm::vec3 Lerp(const glm::vec3& a, const glm::vec3& b, float t) {
        return a + t * (b - a);
    }
}
