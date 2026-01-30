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

#include "glfw_fwd.h"
#include <optional>
#include <utility>

int InitGLFW();
void Terminate();
void InitHint(int hint, int value);
void GetVersion(int* major, int* minor, int* rev);
const char* GetVersionString();

typedef void (*GLFWerrorfun)(int, const char*);
GLFWerrorfun SetErrorCallback(GLFWerrorfun callback);
// const char* GetError(int* error_code);

GLFWwindow* fwCreateWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share);
void DestroyWindow(GLFWwindow* window);
int WindowShouldClose(GLFWwindow* window);
void SetWindowShouldClose(GLFWwindow* window, int value);
void SetWindowTitle(GLFWwindow* window, const char* title);
void SetWindowIcon(GLFWwindow* window, int count, const GLFWimage* images);
void GetWindowPos(GLFWwindow* window, int* xpos, int* ypos);
void SetWindowPos(GLFWwindow* window, int xpos, int ypos);
void GetWindowSize(GLFWwindow* window, int* width, int* height);
void SetWindowSize(GLFWwindow* window, int width, int height);
void SetWindowSizeLimits(GLFWwindow* window, int minwidth, int minheight, int maxwidth, int maxheight);
void SetWindowAspectRatio(GLFWwindow* window, int numer, int denom);
void GetFramebufferSize(GLFWwindow* window, int* width, int* height);
void GetWindowFrameSize(GLFWwindow* window, int* left, int* top, int* right, int* bottom);
void GetWindowContentScale(GLFWwindow* window, float* xscale, float* yscale);
float GetWindowOpacity(GLFWwindow* window);
void SetWindowOpacity(GLFWwindow* window, float opacity);
void IconifyWindow(GLFWwindow* window);
void RestoreWindow(GLFWwindow* window);
void MaximizeWindow(GLFWwindow* window);
void ShowWindow(GLFWwindow* window);
void HideWindow(GLFWwindow* window);
void FocusWindow(GLFWwindow* window);
void RequestWindowAttention(GLFWwindow* window);
GLFWmonitor* GetWindowMonitor(GLFWwindow* window);
void SetWindowMonitor(GLFWwindow* window, GLFWmonitor* monitor, int xpos, int ypos, int width, int height, int refreshRate);
int GetWindowAttrib(GLFWwindow* window, int attrib);
void SetWindowAttrib(GLFWwindow* window, int attrib, int value);
void SetWindowUserPointer(GLFWwindow* window, void* pointer);
void* GetWindowUserPointer(GLFWwindow* window);
void DefaultWindowHints();
void WindowHint(int hint, int value);
void WindowHintString(int hint, const char* value);

GLFWwindowposfun SetWindowPosCallback(GLFWwindow* window, GLFWwindowposfun callback);
GLFWwindowsizefun SetWindowSizeCallback(GLFWwindow* window, GLFWwindowsizefun callback);
GLFWwindowclosefun SetWindowCloseCallback(GLFWwindow* window, GLFWwindowclosefun callback);
GLFWwindowrefreshfun SetWindowRefreshCallback(GLFWwindow* window, GLFWwindowrefreshfun callback);
GLFWwindowfocusfun SetWindowFocusCallback(GLFWwindow* window, GLFWwindowfocusfun callback);
GLFWwindowiconifyfun SetWindowIconifyCallback(GLFWwindow* window, GLFWwindowiconifyfun callback);
GLFWwindowmaximizefun SetWindowMaximizeCallback(GLFWwindow* window, GLFWwindowmaximizefun callback);
GLFWframebuffersizefun SetFramebufferSizeCallback(GLFWwindow* window, GLFWframebuffersizefun callback);
GLFWwindowcontentscalefun SetWindowContentScaleCallback(GLFWwindow* window, GLFWwindowcontentscalefun callback);

// Fixed: Return GLFWwindow* instead of int
GLFWwindow* GetCurrentContext();
GLFWwindow* ContextFromIntPtr(int contextPtr);
GLFWwindow* GetGLFWwindowFromContext();
void CreateContextCurrent(GLFWwindow* window);
void gSwapBuffers(GLFWwindow* window);
void SwapInterval(int interval);
int ExtensionSupported(const char* extension);
// Renamed to avoid conflict with Windows API
GLFWglproc GetGLFWProcAddress(const char* procname);

void PollEvents();
void WaitEvents();
void WaitEventsTimeout(double timeout);
void PostEmptyEvent();

int GetKey(GLFWwindow* window, int key);
GLFWkeyfun SetKeyCallback(GLFWwindow* window, GLFWkeyfun callback);
GLFWcharfun SetCharCallback(GLFWwindow* window, GLFWcharfun callback);
GLFWcharmodsfun SetCharModsCallback(GLFWwindow* window, GLFWcharmodsfun callback);
const char* GetKeyName(int key, int scancode);
int GetKeyScancode(int key);

void GetCursorPos(GLFWwindow* window, double* xpos, double* ypos);
void SetCursorPos(GLFWwindow* window, double xpos, double ypos);
int GetMouseButton(GLFWwindow* window, int button);
GLFWcursorposfun SetCursorPosCallback(GLFWwindow* window, GLFWcursorposfun callback);
GLFWcursorenterfun SetCursorEnterCallback(GLFWwindow* window, GLFWcursorenterfun callback);
GLFWmousebuttonfun SetMouseButtonCallback(GLFWwindow* window, GLFWmousebuttonfun callback);
GLFWscrollfun SetScrollCallback(GLFWwindow* window, GLFWscrollfun callback);

GLFWcursor* CreateCursor(const GLFWimage* image, int xhot, int yhot);
GLFWcursor* CreateStandardCursor(int shape);
void DestroyCursor(GLFWcursor* cursor);
void SetCursor(GLFWwindow* window, GLFWcursor* cursor);
void SetInputMode(GLFWwindow* window, int mode, int value);
int GetInputMode(GLFWwindow* window, int mode);

int JoystickPresent(int jid);
const float* GetJoystickAxes(int jid, int* count);
const unsigned char* GetJoystickButtons(int jid, int* count);
const unsigned char* GetJoystickHats(int jid, int* count);
const char* GetJoystickName(int jid);
const char* GetJoystickGUID(int jid);
void SetJoystickUserPointer(int jid, void* pointer);
void* GetJoystickUserPointer(int jid);
int JoystickIsGamepad(int jid);
GLFWjoystickfun SetJoystickCallback(GLFWjoystickfun callback);
int UpdateGamepadMappings(const char* string);
const char* GetGamepadName(int jid);
int GetGamepadState(int jid, GLFWgamepadstate* state);

void SetClipboardString(GLFWwindow* window, const char* string);
const char* GetClipboardString(GLFWwindow* window);

GLFWdropfun SetDropCallback(GLFWwindow* window, GLFWdropfun callback);

GLFWmonitor** GetMonitors(int* count);
GLFWmonitor* GetPrimaryMonitor();
void GetMonitorPos(GLFWmonitor* monitor, int* xpos, int* ypos);
void GetMonitorWorkarea(GLFWmonitor* monitor, int* xpos, int* ypos, int* width, int* height);
void GetMonitorPhysicalSize(GLFWmonitor* monitor, int* widthMM, int* heightMM);
void GetMonitorContentScale(GLFWmonitor* monitor, float* xscale, float* yscale);
const char* GetMonitorName(GLFWmonitor* monitor);
void SetMonitorUserPointer(GLFWmonitor* monitor, void* pointer);
void* GetMonitorUserPointer(GLFWmonitor* monitor);
GLFWmonitorfun SetMonitorCallback(GLFWmonitorfun callback);
std::pair<const GLFWvidmode*, std::optional<DisplayMode>> GetVideoModeWithDisplayMode(GLFWmonitor* monitor);
const GLFWvidmode* GetVideoModes(GLFWmonitor* monitor, int* count);
void SetGamma(GLFWmonitor* monitor, float gamma);
const GLFWgammaramp* GetGammaRamp(GLFWmonitor* monitor);
void SetGammaRamp(GLFWmonitor* monitor, const GLFWgammaramp* ramp);

int VulkanSupported();
const char** GetRequiredInstanceExtensions(unsigned int* count);

double GetTime();
void SetTime(double time);
unsigned long long GetTimerValue();
unsigned long long GetTimerFrequency();