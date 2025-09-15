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

#include <iostream>
#include <chrono>
#include <unordered_map>
#include <format>

struct TimerResult {
    float allTimes = 0;
    float sampleCount = 0;
};
inline std::unordered_map<std::string, TimerResult> g_timerResults;

class FrameTimer {
private:
    float _lastFrame = 0.0f;
    float _deltaTime = 0.0f;
    float _fps = 0.0f;
    float _fpsUpdateTimer = 0.0f;
    int _frameCount = 0;
public:
    void Update() {
        float currentFrame = static_cast<float>(glfwGetTime());
        _deltaTime = currentFrame - _lastFrame;
        _lastFrame = currentFrame;

        // Cap delta time to prevent large jumps
        if (_deltaTime > 1.0f / 30.0f) { // Cap at 30 FPS minimum
            _deltaTime = 1.0f / 30.0f;
        }

        // Calculate FPS
        _frameCount++;
        _fpsUpdateTimer += _deltaTime;
        if (_fpsUpdateTimer >= 1.0f) {
            _fps = _frameCount / _fpsUpdateTimer;
            _frameCount = 0;
            _fpsUpdateTimer = 0.0f;
        }
    }

    float GetDeltaTime() const { return _deltaTime; }
    float GetFPS() const { return _fps; }
};

struct Timer {
    std::chrono::time_point<std::chrono::steady_clock> m_startTime;
    std::string m_name;

    Timer(const std::string& name) {
        m_startTime = std::chrono::steady_clock::now();
        m_name = name;
    }

    ~Timer() {
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<float, std::milli> duration = now - m_startTime;
        float time = duration.count();

        std::string spacing;
        int extraSpaces = 50 - static_cast<int>(m_name.length());
        for (int i = 0; i < extraSpaces; i++) {
            spacing += " ";
        }

        g_timerResults[m_name].allTimes += time;
        g_timerResults[m_name].sampleCount++;
        float avg = g_timerResults[m_name].allTimes / g_timerResults[m_name].sampleCount;

        std::cout << m_name << ":" << spacing
            << std::format("{:.4f}", time) << "ms      average: "
            << std::format("{:.4f}", avg) << "ms\n";
    }
};