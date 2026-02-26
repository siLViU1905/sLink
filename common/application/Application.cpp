#include "Application.h"

namespace sLink::application
{
    Application::Application(int windowWidth, int windowHeight, std::string_view windowName) : m_Window(
        windowWidth, windowHeight, windowName)
    {
        m_Renderer.init(m_Window);
    }

    void Application::run()
    {
        while (m_Window.isOpen())
        {
            m_Window.pollEvents();

            onUpdate();

            if (m_Window.isMinimized())
                continue;

            onRender();

            if (m_Window.wasResized())
                handleWindowResize();
        }

        m_Renderer.waitIdle();
    }

    void Application::handleWindowResize()
    {
        m_Renderer.updateSwapChain(m_Window);

        int width = m_Window.getWidth();

        int height = m_Window.getHeight();

        m_UIBackend.updateDisplay(width, height);
    }
}