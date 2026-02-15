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

    protected:
        window::Window m_Window;

        ui::UIBackend m_UIBackend;

        renderer::Renderer m_Renderer;

        virtual void onUpdate() = 0;

        virtual void onRender() = 0;

        virtual void onRenderUI() = 0;

    private:
        void handleWindowResize();
    };
}

#endif