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

#ifndef TIMER_H
#define TIMER_H

#include "common.h"

struct TimerResult {
    static constexpr size_t MAX_SAMPLES = 1000;
    float allTimes = 0.0f;
    float sampleCount = 0.0f;
    float minTime = std::numeric_limits<float>::max();
    float maxTime = 0.0f;

    void AddSample(float time) {
        minTime = std::min(minTime, time);
        maxTime = std::max(maxTime, time);

        if (sampleCount >= MAX_SAMPLES) {
            allTimes = (allTimes * MAX_SAMPLES - allTimes / MAX_SAMPLES) / (MAX_SAMPLES - 1);
            sampleCount = MAX_SAMPLES - 1;
        }
        allTimes += time;
        sampleCount++;
    }

    float GetAverage() const {
        return sampleCount > 0 ? allTimes / sampleCount : 0.0f;
    }

    void Reset() {
        allTimes = 0.0f;
        sampleCount = 0.0f;
        minTime = std::numeric_limits<float>::max();
        maxTime = 0.0f;
    }
};

inline std::unordered_map<std::string, TimerResult> g_timerResults;
inline std::mutex g_timerMutex;

#ifndef PROJECT32_ENABLE_PROFILING
#define PROJECT32_ENABLE_PROFILING 1
#endif

class FrameTimer {
private:
    float _startTime = 0.0f;
    float _lastFrame = 0.0f;
    float _deltaTime = 0.0f;
    float _fps = 0.0f;
    float _fpsUpdateTimer = 0.0f;
    int _frameCount = 0;
    float _minDeltaTime = std::numeric_limits<float>::max();
    float _maxDeltaTime = 0.0f;

public:
    FrameTimer() {
        float currentTime = static_cast<float>(glfwGetTime());
        _startTime = currentTime;
        _lastFrame = currentTime;
    }

    void Update() {
        float currentFrame = static_cast<float>(glfwGetTime());
        _deltaTime = currentFrame - _lastFrame;
        _lastFrame = currentFrame;

        if (_deltaTime > 1.0f / 30.0f) { 
            _deltaTime = 1.0f / 30.0f;
        }

        _minDeltaTime = std::min(_minDeltaTime, _deltaTime);
        _maxDeltaTime = std::max(_maxDeltaTime, _deltaTime);

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
    float GetMinDeltaTime() const { return _minDeltaTime; }
    float GetMaxDeltaTime() const { return _maxDeltaTime; }
    float GetElapsedTime() const { return static_cast<float>(glfwGetTime()) - _startTime; }

    void Reset() {
        float currentTime = static_cast<float>(glfwGetTime());
        _startTime = currentTime;
        _lastFrame = currentTime;
        _deltaTime = 0.0f;
        _fps = 0.0f;
        _fpsUpdateTimer = 0.0f;
        _frameCount = 0;
        _minDeltaTime = std::numeric_limits<float>::max();
        _maxDeltaTime = 0.0f;
    }
};

struct Timer {
    std::chrono::time_point<std::chrono::steady_clock> m_startTime;
    std::string m_name;

    Timer(std::string_view name) : m_name(name) {
#if PROJECT32_ENABLE_PROFILING
        m_startTime = std::chrono::steady_clock::now();
#endif
    }

    ~Timer() {
#if PROJECT32_ENABLE_PROFILING
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<float, std::milli> duration = now - m_startTime;
        float time = duration.count();

        int extraSpaces = std::max(0, 50 - static_cast<int>(m_name.length()));
        std::string spacing(extraSpaces, ' ');

        {
            std::lock_guard<std::mutex> lock(g_timerMutex);
            g_timerResults[m_name].AddSample(time);
        }

        const auto& result = g_timerResults[m_name];
        std::cout << m_name << ":" << spacing
            << std::format("{:.4f}", time) << "ms      average: "
            << std::format("{:.4f}", result.GetAverage()) << "ms      min: "
            << std::format("{:.4f}", result.minTime) << "ms      max: "
            << std::format("{:.4f}", result.maxTime) << "ms\n";
#endif
    }
};

inline void ResetAllTimers() {
    std::lock_guard<std::mutex> lock(g_timerMutex);
    for (auto& [name, result] : g_timerResults) {
        result.Reset();
    }
}

inline void ResetTimer(std::string_view name) {
    std::lock_guard<std::mutex> lock(g_timerMutex);
    auto it = g_timerResults.find(std::string(name));
    if (it != g_timerResults.end()) {
        it->second.Reset();
    }
}

inline void PrintTimerSummary() {
    std::lock_guard<std::mutex> lock(g_timerMutex);
    std::cout << "\n=== Timer Summary ===\n";
    for (const auto& [name, result] : g_timerResults) {
        int extraSpaces = std::max(0, 50 - static_cast<int>(name.length()));
        std::string spacing(extraSpaces, ' ');
        std::cout << name << ":" << spacing
            << "avg: " << std::format("{:.4f}", result.GetAverage()) << "ms    "
            << "min: " << std::format("{:.4f}", result.minTime) << "ms    "
            << "max: " << std::format("{:.4f}", result.maxTime) << "ms    "
            << "samples: " << static_cast<int>(result.sampleCount) << "\n";
    }
    std::cout << "=====================\n\n";
}

#endif // TIMER_H