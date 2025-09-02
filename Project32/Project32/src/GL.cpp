#include "GL.h"

namespace GL {
    GLFWwindow* _window = nullptr;
    int _windowWidth = 1920;
    int _windowHeight = 1080;
    bool _vsync = true;

    void SetVSync(bool enabled) {
        _vsync = enabled;
        glfwSwapInterval(enabled ? 1 : 0);
    }

    void ToggleFullscreen() {
        static bool isFullscreen = false;
        static int windowedWidth = _windowWidth;
        static int windowedHeight = _windowHeight;
        static int windowedPosX = 100;
        static int windowedPosY = 100;

        if (isFullscreen) {
            glfwSetWindowMonitor(_window, nullptr, windowedPosX, windowedPosY,
                windowedWidth, windowedHeight, 0);
        }
        else {
            glfwGetWindowPos(_window, &windowedPosX, &windowedPosY);
            glfwGetWindowSize(_window, &windowedWidth, &windowedHeight);
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwSetWindowMonitor(_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }
        isFullscreen = !isFullscreen;
    }

    void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
        _windowWidth = width;
        _windowHeight = height;
        glViewport(0, 0, width, height);
    }

    void Init(int width, int height, std::string title) {
        _windowWidth = width;
        _windowHeight = height;

        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
        glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA

        _window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (!_window) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(_window);
        glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetFramebufferSizeCallback(_window, FramebufferSizeCallback);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            throw std::runtime_error("Failed to initialize GLAD");
        }

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        glDisable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glViewport(0, 0, width, height);

        SetVSync(true);

        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
        std::cout << "GPU: " << glGetString(GL_RENDERER) << std::endl;
    }

    GLFWwindow* GetWindowPointer() { return _window; }
    bool WindowIsOpen() { return !glfwWindowShouldClose(_window); }
    void SetWindowShouldClose(bool value) { glfwSetWindowShouldClose(_window, value); }

    void SwapBuffersPollEvents() {
        glfwSwapBuffers(_window);
        glfwPollEvents();
    }

    void Cleanup() {
        if (_window) {
            glfwDestroyWindow(_window);
        }
        glfwTerminate();
    }

    glm::ivec2 GetWindowSize() { return glm::ivec2(_windowWidth, _windowHeight); }
}
