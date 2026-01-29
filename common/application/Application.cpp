#include "Application.h"

namespace sLink::application
{
    Application::Application(window::Window &&window):m_Window(std::move(window))
    {
        m_Renderer.init(m_Window);
    }
}
