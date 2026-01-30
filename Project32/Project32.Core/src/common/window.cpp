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

#include "../platform/glfw_config.h"
#include "window.h"
#include <cstdint>
#include <optional>
#include <utility>

int InitGLFW() {
    return glfwInit();
}

void Terminate() {
    glfwTerminate();
}

void InitHint(int hint, int value) {
    glfwInitHint(hint, value);
}

void GetVersion(int* major, int* minor, int* rev) {
    glfwGetVersion(major, minor, rev);
}

const char* GetVersionString() {
    return glfwGetVersionString();
}

GLFWerrorfun SetErrorCallback(GLFWerrorfun callback) {
    return glfwSetErrorCallback(callback);
}

/*
const char* GetError(int* error_code) {
    return glfwGetError(error_code);
}
*/

GLFWwindow* fwCreateWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share) {
    return glfwCreateWindow(width, height, title, monitor, share);
}

void DestroyWindow(GLFWwindow* window) {
    glfwDestroyWindow(window);
}

int WindowShouldClose(GLFWwindow* window) {
    return glfwWindowShouldClose(window);
}

void SetWindowShouldClose(GLFWwindow* window, int value) {
    glfwSetWindowShouldClose(window, value);
}

void SetWindowTitle(GLFWwindow* window, const char* title) {
    glfwSetWindowTitle(window, title);
}

void SetWindowIcon(GLFWwindow* window, int count, const GLFWimage* images) {
    glfwSetWindowIcon(window, count, images);
}

void GetWindowPos(GLFWwindow* window, int* xpos, int* ypos) {
    glfwGetWindowPos(window, xpos, ypos);
}

void SetWindowPos(GLFWwindow* window, int xpos, int ypos) {
    glfwSetWindowPos(window, xpos, ypos);
}

void GetWindowSize(GLFWwindow* window, int* width, int* height) {
    glfwGetWindowSize(window, width, height);
}

void SetWindowSize(GLFWwindow* window, int width, int height) {
    glfwSetWindowSize(window, width, height);
}

void SetWindowSizeLimits(GLFWwindow* window, int minwidth, int minheight, int maxwidth, int maxheight) {
    glfwSetWindowSizeLimits(window, minwidth, minheight, maxwidth, maxheight);
}

void SetWindowAspectRatio(GLFWwindow* window, int numer, int denom) {
    glfwSetWindowAspectRatio(window, numer, denom);
}

void GetFramebufferSize(GLFWwindow* window, int* width, int* height) {
    glfwGetFramebufferSize(window, width, height);
}

void GetWindowFrameSize(GLFWwindow* window, int* left, int* top, int* right, int* bottom) {
    glfwGetWindowFrameSize(window, left, top, right, bottom);
}

void GetWindowContentScale(GLFWwindow* window, float* xscale, float* yscale) {
    glfwGetWindowContentScale(window, xscale, yscale);
}

float GetWindowOpacity(GLFWwindow* window) {
    return glfwGetWindowOpacity(window);
}

void SetWindowOpacity(GLFWwindow* window, float opacity) {
    glfwSetWindowOpacity(window, opacity);
}

void IconifyWindow(GLFWwindow* window) {
    glfwIconifyWindow(window);
}

void RestoreWindow(GLFWwindow* window) {
    glfwRestoreWindow(window);
}

void MaximizeWindow(GLFWwindow* window) {
    glfwMaximizeWindow(window);
}

void ShowWindow(GLFWwindow* window) {
    glfwShowWindow(window);
}

void HideWindow(GLFWwindow* window) {
    glfwHideWindow(window);
}

void FocusWindow(GLFWwindow* window) {
    glfwFocusWindow(window);
}

void RequestWindowAttention(GLFWwindow* window) {
    glfwRequestWindowAttention(window);
}

GLFWmonitor* GetWindowMonitor(GLFWwindow* window) {
    return glfwGetWindowMonitor(window);
}

void SetWindowMonitor(GLFWwindow* window, GLFWmonitor* monitor, int xpos, int ypos, int width, int height, int refreshRate) {
    glfwSetWindowMonitor(window, monitor, xpos, ypos, width, height, refreshRate);
}

int GetWindowAttrib(GLFWwindow* window, int attrib) {
    return glfwGetWindowAttrib(window, attrib);
}

void SetWindowAttrib(GLFWwindow* window, int attrib, int value) {
    glfwSetWindowAttrib(window, attrib, value);
}

void SetWindowUserPointer(GLFWwindow* window, void* pointer) {
    glfwSetWindowUserPointer(window, pointer);
}

void* GetWindowUserPointer(GLFWwindow* window) {
    return glfwGetWindowUserPointer(window);
}

void DefaultWindowHints() {
    glfwDefaultWindowHints();
}

void WindowHint(int hint, int value) {
    glfwWindowHint(hint, value);
}

void WindowHintString(int hint, const char* value) {
    glfwWindowHintString(hint, value);
}

GLFWwindowposfun SetWindowPosCallback(GLFWwindow* window, GLFWwindowposfun callback) {
    return glfwSetWindowPosCallback(window, callback);
}

GLFWwindowsizefun SetWindowSizeCallback(GLFWwindow* window, GLFWwindowsizefun callback) {
    return glfwSetWindowSizeCallback(window, callback);
}

GLFWwindowclosefun SetWindowCloseCallback(GLFWwindow* window, GLFWwindowclosefun callback) {
    return glfwSetWindowCloseCallback(window, callback);
}

GLFWwindowrefreshfun SetWindowRefreshCallback(GLFWwindow* window, GLFWwindowrefreshfun callback) {
    return glfwSetWindowRefreshCallback(window, callback);
}

GLFWwindowfocusfun SetWindowFocusCallback(GLFWwindow* window, GLFWwindowfocusfun callback) {
    return glfwSetWindowFocusCallback(window, callback);
}

GLFWwindowiconifyfun SetWindowIconifyCallback(GLFWwindow* window, GLFWwindowiconifyfun callback) {
    return glfwSetWindowIconifyCallback(window, callback);
}

GLFWwindowmaximizefun SetWindowMaximizeCallback(GLFWwindow* window, GLFWwindowmaximizefun callback) {
    return glfwSetWindowMaximizeCallback(window, callback);
}

GLFWframebuffersizefun SetFramebufferSizeCallback(GLFWwindow* window, GLFWframebuffersizefun callback) {
    return glfwSetFramebufferSizeCallback(window, callback);
}

GLFWwindowcontentscalefun SetWindowContentScaleCallback(GLFWwindow* window, GLFWwindowcontentscalefun callback) {
    return glfwSetWindowContentScaleCallback(window, callback);
}

// Fixed: Return GLFWwindow* directly instead of casting to int
GLFWwindow* GetCurrentContext() {
    return glfwGetCurrentContext();
}

GLFWwindow* ContextFromIntPtr(int contextPtr) {
    return reinterpret_cast<GLFWwindow*>(static_cast<uintptr_t>(contextPtr));
}

GLFWwindow* GetGLFWwindowFromContext() {
    return glfwGetCurrentContext();
}

void CreateContextCurrent(GLFWwindow* window) {
    glfwMakeContextCurrent(window);
}

void gSwapBuffers(GLFWwindow* window) {
    glfwSwapBuffers(window);
}

void SwapInterval(int interval) {
    glfwSwapInterval(interval);
}

int ExtensionSupported(const char* extension) {
    return glfwExtensionSupported(extension);
}

GLFWglproc GetGLFWProcAddress(const char* procname) {
    return glfwGetProcAddress(procname);
}

void PollEvents() {
    glfwPollEvents();
}

void WaitEvents() {
    glfwWaitEvents();
}

void WaitEventsTimeout(double timeout) {
    glfwWaitEventsTimeout(timeout);
}

void PostEmptyEvent() {
    glfwPostEmptyEvent();
}

int GetKey(GLFWwindow* window, int key) {
    return glfwGetKey(window, key);
}

GLFWkeyfun SetKeyCallback(GLFWwindow* window, GLFWkeyfun callback) {
    return glfwSetKeyCallback(window, callback);
}

GLFWcharfun SetCharCallback(GLFWwindow* window, GLFWcharfun callback) {
    return glfwSetCharCallback(window, callback);
}

GLFWcharmodsfun SetCharModsCallback(GLFWwindow* window, GLFWcharmodsfun callback) {
    return glfwSetCharModsCallback(window, callback);
}

const char* GetKeyName(int key, int scancode) {
    return glfwGetKeyName(key, scancode);
}

int GetKeyScancode(int key) {
    return glfwGetKeyScancode(key);
}

void GetCursorPos(GLFWwindow* window, double* xpos, double* ypos) {
    glfwGetCursorPos(window, xpos, ypos);
}

void SetCursorPos(GLFWwindow* window, double xpos, double ypos) {
    glfwSetCursorPos(window, xpos, ypos);
}

int GetMouseButton(GLFWwindow* window, int button) {
    return glfwGetMouseButton(window, button);
}

GLFWcursorposfun SetCursorPosCallback(GLFWwindow* window, GLFWcursorposfun callback) {
    return glfwSetCursorPosCallback(window, callback);
}

GLFWcursorenterfun SetCursorEnterCallback(GLFWwindow* window, GLFWcursorenterfun callback) {
    return glfwSetCursorEnterCallback(window, callback);
}

GLFWmousebuttonfun SetMouseButtonCallback(GLFWwindow* window, GLFWmousebuttonfun callback) {
    return glfwSetMouseButtonCallback(window, callback);
}

GLFWscrollfun SetScrollCallback(GLFWwindow* window, GLFWscrollfun callback) {
    return glfwSetScrollCallback(window, callback);
}

GLFWcursor* CreateCursor(const GLFWimage* image, int xhot, int yhot) {
    return glfwCreateCursor(image, xhot, yhot);
}

GLFWcursor* CreateStandardCursor(int shape) {
    return glfwCreateStandardCursor(shape);
}

void DestroyCursor(GLFWcursor* cursor) {
    glfwDestroyCursor(cursor);
}

void SetCursor(GLFWwindow* window, GLFWcursor* cursor) {
    glfwSetCursor(window, cursor);
}

void SetInputMode(GLFWwindow* window, int mode, int value) {
    glfwSetInputMode(window, mode, value);
}

int GetInputMode(GLFWwindow* window, int mode) {
    return glfwGetInputMode(window, mode);
}

int JoystickPresent(int jid) {
    return glfwJoystickPresent(jid);
}

const float* GetJoystickAxes(int jid, int* count) {
    return glfwGetJoystickAxes(jid, count);
}

const unsigned char* GetJoystickButtons(int jid, int* count) {
    return glfwGetJoystickButtons(jid, count);
}

const unsigned char* GetJoystickHats(int jid, int* count) {
    return glfwGetJoystickHats(jid, count);
}

const char* GetJoystickName(int jid) {
    return glfwGetJoystickName(jid);
}

const char* GetJoystickGUID(int jid) {
    return glfwGetJoystickGUID(jid);
}

void SetJoystickUserPointer(int jid, void* pointer) {
    glfwSetJoystickUserPointer(jid, pointer);
}

void* GetJoystickUserPointer(int jid) {
    return glfwGetJoystickUserPointer(jid);
}

int JoystickIsGamepad(int jid) {
    return glfwJoystickIsGamepad(jid);
}

GLFWjoystickfun SetJoystickCallback(GLFWjoystickfun callback) {
    return glfwSetJoystickCallback(callback);
}

int UpdateGamepadMappings(const char* string) {
    return glfwUpdateGamepadMappings(string);
}

const char* GetGamepadName(int jid) {
    return glfwGetGamepadName(jid);
}

int GetGamepadState(int jid, GLFWgamepadstate* state) {
    return glfwGetGamepadState(jid, state);
}

void SetClipboardString(GLFWwindow* window, const char* string) {
    glfwSetClipboardString(window, string);
}

const char* GetClipboardString(GLFWwindow* window) {
    return glfwGetClipboardString(window);
}

GLFWdropfun SetDropCallback(GLFWwindow* window, GLFWdropfun callback) {
    return glfwSetDropCallback(window, callback);
}

GLFWmonitor** GetMonitors(int* count) {
    return glfwGetMonitors(count);
}

GLFWmonitor* GetPrimaryMonitor() {
    return glfwGetPrimaryMonitor();
}

void GetMonitorPos(GLFWmonitor* monitor, int* xpos, int* ypos) {
    glfwGetMonitorPos(monitor, xpos, ypos);
}

void GetMonitorWorkarea(GLFWmonitor* monitor, int* xpos, int* ypos, int* width, int* height) {
    glfwGetMonitorWorkarea(monitor, xpos, ypos, width, height);
}

void GetMonitorPhysicalSize(GLFWmonitor* monitor, int* widthMM, int* heightMM) {
    glfwGetMonitorPhysicalSize(monitor, widthMM, heightMM);
}

void GetMonitorContentScale(GLFWmonitor* monitor, float* xscale, float* yscale) {
    glfwGetMonitorContentScale(monitor, xscale, yscale);
}

const char* GetMonitorName(GLFWmonitor* monitor) {
    return glfwGetMonitorName(monitor);
}

void SetMonitorUserPointer(GLFWmonitor* monitor, void* pointer) {
    glfwSetMonitorUserPointer(monitor, pointer);
}

void* GetMonitorUserPointer(GLFWmonitor* monitor) {
    return glfwGetMonitorUserPointer(monitor);
}

GLFWmonitorfun SetMonitorCallback(GLFWmonitorfun callback) {
    return glfwSetMonitorCallback(callback);
}

const GLFWvidmode* GetVideoModes(GLFWmonitor* monitor, int* count) {
    return glfwGetVideoModes(monitor, count);
}

std::pair<const GLFWvidmode*, std::optional<DisplayMode>>
GetVideoModeWithDisplayMode(GLFWmonitor* monitor)
{
    if (!monitor)
        return { nullptr, std::nullopt };

    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (!mode)
        return { nullptr, std::nullopt };

    DisplayMode Mode{
        mode->width,
        mode->height,
		mode->refreshRate
    };

    return { mode, Mode };
}

void SetGamma(GLFWmonitor* monitor, float gamma) {
    glfwSetGamma(monitor, gamma);
}

const GLFWgammaramp* GetGammaRamp(GLFWmonitor* monitor) {
    return glfwGetGammaRamp(monitor);
}

void SetGammaRamp(GLFWmonitor* monitor, const GLFWgammaramp* ramp) {
    glfwSetGammaRamp(monitor, ramp);
}

int VulkanSupported() {
    return glfwVulkanSupported();
}

const char** GetRequiredInstanceExtensions(unsigned int* count) {
    return glfwGetRequiredInstanceExtensions(count);
}

double GetTime() {
    return glfwGetTime();
}

void SetTime(double time) {
    glfwSetTime(time);
}

unsigned long long GetTimerValue() {
    return glfwGetTimerValue();
}

unsigned long long GetTimerFrequency() {
    return glfwGetTimerFrequency();
}