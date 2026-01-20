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

#define WIN32_LEAN_AND_MEAN    
#define NOMINMAX
#include <windows.h>

#include "types/constants.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <glad/glad.h>

#include "common/glfw_fwd.h"
#include "common/time.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <spdlog/spdlog.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <memory>
#include <thread>
#include <utility>
#include <map>
#include <array>
#include <cfloat>
#include <optional>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <iomanip>
#include <functional>
#include <format>
#include <string_view>
#include <mutex>
#include <limits>
#include <atomic>
#include <condition_variable>
#include <queue>
#include <variant>

#include <tiny_obj_loader.h>

#include "stb_image.h"

#include "math.h"

#include "types/util.hpp"
#include "types/timer.hpp"
#include "types/math.h"
#include "BackEnd/common.h"

#include "scene/transform.h"
#include "renderer/vertex.h"