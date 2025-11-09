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

#define GLM_FORCE_SILENT_WARNINGS
#define GLM_ENABLE_EXPERIMENTAL

#define NEAR_PLANE 0.1f
#define FAR_PLANE 1000.0f
#define DARK_SQUARE glm::vec3(0.3984375f, 0.265625f, 0.2265625f)
#define LIGHT_SQUARE glm::vec3(0.95703125f, 0.8984375f, 0.74609375f)
#define MAX_BONE_INFLUENCE 4

#define PI 3.14159265359f
#define PI_2 (PI / 2.0f)
#define PI_4 (PI / 4.0f)
#define TAU (2.0f * PI)