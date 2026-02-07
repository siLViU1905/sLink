#include "ClientApplication.h"

namespace sLink::client_application
{
	ClientApplication::ClientApplication(int windowWidth, int windowHeight, std::string_view windowName):
		application::Application(windowWidth, windowHeight, windowName)
	{
	}

	void ClientApplication::onUpdate()
	{
	}

	void ClientApplication::onRender()
	{
		m_Renderer.waitForFences();

		m_Renderer.renderFrame();
	}
}