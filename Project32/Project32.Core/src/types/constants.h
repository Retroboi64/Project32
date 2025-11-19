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

#include <limits>
#include <format>
#include <algorithm>
#include <glm/glm.hpp>

#define GLM_FORCE_SILENT_WARNINGS
#define GLM_ENABLE_EXPERIMENTAL

namespace P32 {
    namespace Engine {
        constexpr int PATCH_VERSION = 1;
		constexpr int MINOR_VERSION = 0;
		constexpr int MAJOR_VERSION = 0;

        inline std::string GetVersionString() {
            return std::format("{}.{}.{}", PATCH_VERSION, MINOR_VERSION, MAJOR_VERSION);
		}
    }

    namespace Constants {
		// TODO: PUT CONSTANTS HERE
    }

    // Mathematical Constants
    constexpr float PI = 3.14159265359f;
    constexpr float PI_2 = PI / 2.0f;
    constexpr float PI_4 = PI / 4.0f;
    constexpr float TAU = 2.0f * PI;
    constexpr float EPSILON = 0.00001f;

    // Vector Constants
    inline const glm::vec3 V3_ZERO{ 0.0f, 0.0f, 0.0f };
    inline const glm::vec3 V3_ONE{ 1.0f, 1.0f, 1.0f };
    inline const glm::vec3 V3_UNIT_X{ 1.0f, 0.0f, 0.0f };
    inline const glm::vec3 V3_UNIT_Y{ 0.0f, 1.0f, 0.0f };
    inline const glm::vec3 V3_UNIT_Z{ 0.0f, 0.0f, 1.0f };
    inline const glm::vec3 V3_UP{ 0.0f, 1.0f, 0.0f };
    inline const glm::vec3 V3_DOWN{ 0.0f, -1.0f, 0.0f };
    inline const glm::vec3 V3_RIGHT{ 1.0f, 0.0f, 0.0f };
    inline const glm::vec3 V3_LEFT{ -1.0f, 0.0f, 0.0f };
    inline const glm::vec3 V3_FORWARD{ 0.0f, 0.0f, -1.0f };
    inline const glm::vec3 V3_BACKWARD{ 0.0f, 0.0f, 1.0f };

    // Projection Constants
    constexpr float NEAR_PLANE = 0.1f;
    constexpr float FAR_PLANE = 1000.0f;

    // Color Constants
    inline const glm::vec3 DARK_SQUARE{ 0.3984375f, 0.265625f, 0.2265625f };
    inline const glm::vec3 LIGHT_SQUARE{ 0.95703125f, 0.8984375f, 0.74609375f };

    // Animation Constants
    constexpr int MAX_BONE_INFLUENCE = 4;

    // Numeric Limits
    inline constexpr float FLOAT_MAX_VAL = std::numeric_limits<float>::max();
    inline constexpr float FLOAT_MIN_VAL = std::numeric_limits<float>::lowest();

    inline constexpr int INT_MAX_VAL = std::numeric_limits<int>::max();
    inline constexpr int INT_MIN_VAL = std::numeric_limits<int>::min();

    inline constexpr unsigned int UINT_MAX_VAL = std::numeric_limits<unsigned int>::max();
    inline constexpr unsigned int UINT_MIN_VAL = std::numeric_limits<unsigned int>::min();

    inline constexpr short SHORT_MAX_VAL = std::numeric_limits<short>::max();
    inline constexpr short SHORT_MIN_VAL = std::numeric_limits<short>::min();

    inline constexpr unsigned short USHORT_MAX_VAL = std::numeric_limits<unsigned short>::max();
    inline constexpr unsigned short USHORT_MIN_VAL = std::numeric_limits<unsigned short>::min();

    inline constexpr unsigned char BYTE_MAX_VAL = std::numeric_limits<unsigned char>::max();
    inline constexpr unsigned char BYTE_MIN_VAL = std::numeric_limits<unsigned char>::min();

    // Math Utility Functions
    constexpr float DegToRad(float degrees) {
        return degrees * (PI / 180.0f);
    }

    constexpr float RadToDeg(float radians) {
        return radians * (180.0f / PI);
    }

    constexpr float ByteToFloat(unsigned char byte) {
        return static_cast<float>(byte) / static_cast<float>(BYTE_MAX_VAL);
    }

    inline unsigned char FloatToByte(float value) {
        return static_cast<unsigned char>(glm::clamp(value, 0.0f, 1.0f) * static_cast<float>(BYTE_MAX_VAL));
    }

    constexpr bool IsPowerOfTwo(size_t x) {
        return (x != 0) && ((x & (x - 1)) == 0);
    }

    constexpr size_t AlignTo(size_t value, size_t alignment) {
        return ((value)+((alignment)-1)) & ~((alignment)-1);
    }

    template<typename T>
    inline void Swap(T& a, T& b) {
        T temp = std::move(a);
        a = std::move(b);
        b = std::move(temp);
    }

    template<typename T, size_t N>
    constexpr size_t ArraySize(T(&)[N]) {
        return N;
    }

    constexpr unsigned int Bit(unsigned int x) {
        return 1u << x;
    }

} // namespace P32