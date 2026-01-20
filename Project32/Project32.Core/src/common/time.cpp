#pragma once

#include "time.h"
#include "../platform/glfw_config.h"

double EngineTime()
{
    return glfwGetTime();
}