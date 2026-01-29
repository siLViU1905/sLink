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

        bool isOpen() const;

        void close();

        GLFWwindow *getGLFWWindow();

        bool wasResized();

        void maximize();

        bool isMaximized() const;

        void restore();

        void resize(int width, int height);

        constexpr int getHeight() const { return height; }

        constexpr int getWidth() const { return width; }

        void getFramebufferSize(int &width, int &height) const;

        void getCursorDeltaPos(double &deltaX, double &deltaY);

        void disableCursor();

        void enableCursor();

        void pollEvents() const;

        void waitForEvents() const;

        ~Window();

    private:
        GLFWwindow *windowHandle;

        int width;

        int height;

        bool framebufferResized;

        static void glfw_framebufferCallback(GLFWwindow *window, int width, int height);
    };
}

#endif //SLINK_WINDOW_H
