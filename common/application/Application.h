#ifndef SLINK_APPLICATION_H
#define SLINK_APPLICATION_H
#include "Renderer.h"
#include "Window.h"

namespace sLink::application
{
    class Application
    {
    public:
        Application(int windowWidth, int windowHeight, std::string_view windowName);

        void run();
    private:
        window::Window m_Window;

        ui::UIBackend m_UIBackend;

        renderer::Renderer m_Renderer;

        void render();

        void handleWindowResize();
    };
}

#endif
