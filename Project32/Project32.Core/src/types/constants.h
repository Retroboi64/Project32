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
    }
} // namespace P32