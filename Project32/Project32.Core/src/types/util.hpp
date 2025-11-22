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

#ifndef UTIL_H
#define UTIL_H

#include "../common.h"
#include "constants.h"

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

    inline bool FileExists(const std::string& name) {
        struct stat buffer;
        return (stat(name.c_str(), &buffer) == 0);
    }

    namespace Math {
        inline float Clamp(float value, float min, float max) {
            if (value < min) return min;
            if (value > max) return max;
            return value;
        }

        // Math Utility Functions
        constexpr float DegToRad(float degrees) {
            return degrees * (P32::Constants::PI / 180.0f);
        }

        constexpr float RadToDeg(float radians) {
            return radians * (180.0f / P32::Constants::PI);
        }

        constexpr float ByteToFloat(unsigned char byte) {
            return static_cast<float>(byte) / static_cast<float>(P32::Constants::BYTE_MAX_VAL);
        }

        inline unsigned char FloatToByte(float value) {
            return static_cast<unsigned char>(glm::clamp(value, 0.0f, 1.0f) * static_cast<float>(P32::Constants::BYTE_MAX_VAL));
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
	}
}

#endif // UTIL_H