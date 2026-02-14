#ifndef SLINK_WINDOW_H
#define SLINK_WINDOW_H

#include <string_view>
#include <GLFW/glfw3.h>

namespace sLink::window
{
    class Window
    {
    public:
        Window(int width, int height, std::string_view title);

        Window(Window &&other) noexcept;

        bool isOpen() const;

        void close();

        GLFWwindow *getGLFWWindow();

        bool wasResized();

        void maximize();

        bool isMaximized() const;

        bool isMinimized() const;

        void restore();

        void resize(int width, int height);

        constexpr int getHeight() const { return m_Height; }

        constexpr int getWidth() const { return m_Width; }

        void getFramebufferSize(int &width, int &height) const;

        void pollEvents() const;

        void waitForEvents() const;

        ~Window();

    private:
        GLFWwindow *m_WindowHandle;

        int m_Width;

        int m_Height;

        bool m_FramebufferResized;

        static void glfw_framebufferCallback(GLFWwindow *window, int width, int height);
    };
}

#endif //SLINK_WINDOW_H