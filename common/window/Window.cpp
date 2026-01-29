#include "Window.h"

#include <stdexcept>

namespace sLink::window
{
    GLFWwindow *Window::getGLFWWindow()
    {
        return windowHandle;
    }

    Window::Window(int width, int height, std::string_view title) : width(width), height(height),
                                                                    framebufferResized(false)
    {
        if (!glfwInit())
            throw std::runtime_error("Failed to init GLFW");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        windowHandle = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);

        if (!windowHandle)
            throw std::runtime_error("Failed to create GLFW Window");

        glfwSetWindowUserPointer(windowHandle, this);

        glfwSetFramebufferSizeCallback(windowHandle, glfw_framebufferCallback);
    }

    bool Window::isOpen() const
    {
        return !glfwWindowShouldClose(windowHandle);
    }

    void Window::close()
    {
        glfwSetWindowShouldClose(windowHandle, true);
    }

    bool Window::wasResized()
    {
        bool temp = framebufferResized;

        framebufferResized = false;

        return temp;
    }

    void Window::maximize()
    {
        glfwMaximizeWindow(windowHandle);
    }

    bool Window::isMaximized() const
    {
        return static_cast<bool>(glfwGetWindowAttrib(windowHandle, GLFW_MAXIMIZED));
    }

    void Window::restore()
    {
        glfwRestoreWindow(windowHandle);
    }

    void Window::resize(int width, int height)
    {
        glfwSetWindowSize(windowHandle, width, height);
    }

    void Window::getFramebufferSize(int &width, int &height) const
    {
        glfwGetFramebufferSize(windowHandle, &width, &height);
    }

    void Window::pollEvents() const
    {
        glfwPollEvents();
    }

    void Window::waitForEvents() const
    {
        glfwWaitEvents();
    }

    void Window::glfw_framebufferCallback(GLFWwindow *window, int width, int height)
    {
        auto *self = static_cast<Window *>(glfwGetWindowUserPointer(window));

        self->framebufferResized = true;

        self->width = width;

        self->height = height;
    }

    Window::~Window()
    {
        glfwDestroyWindow(windowHandle);

        glfwTerminate();
    }
}
