#include "ClientApplication.h"

namespace sLink::client_application
{
	ClientApplication::ClientApplication(int windowWidth, int windowHeight, std::string_view windowName):
		application::Application(windowWidth, windowHeight, windowName),
		m_Client(m_IOContext)
	{
		initLayers();

		m_Client.connect("127.0.0.1", "12444");

		m_NetworkThread = std::jthread([this]()
			{
				m_IOContext.run();
			});
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