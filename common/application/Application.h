#ifndef SLINK_APPLICATION_H
#define SLINK_APPLICATION_H
#include "Renderer.h"
#include "Window.h"

namespace sLink::application
{
    class Application
    {
    public:
        Application(window::Window&& window);

    private:
        window::Window m_Window;

        renderer::Renderer m_Renderer;

        ui::UIBackend m_UIBackend;
    };
}

#endif
