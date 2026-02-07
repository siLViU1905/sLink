#include "ServerApplication.h"

namespace sLink::server_application
{
	ServerApplication::ServerApplication(int windowWidth, int windowHeight, std::string_view windowName):
		application::Application(windowWidth, windowHeight, windowName)
	{
	}

	void ServerApplication::onUpdate()
	{
	}

	void ServerApplication::onRender()
	{
		m_Renderer.waitForFences();

		m_Renderer.renderFrame();
	}
}