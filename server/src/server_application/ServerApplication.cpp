#include "ServerApplication.h"

namespace sLink::server_application
{
	ServerApplication::ServerApplication(int windowWidth, int windowHeight, std::string_view windowName) :
		application::Application(windowWidth, windowHeight, windowName),
		m_Server(m_IOContext, 12444)
	{
		m_NetworkThread = std::jthread([this]()
			{
				m_IOContext.run();
			});

		initLayers();
	}

	ServerApplication::~ServerApplication()
	{
		m_IOContext.stop();
	}

	void ServerApplication::onUpdate()
	{
		m_Server.update();

		onUpdateConnectedClients();

		onUpdateDisconnectedClients();
	}

	void ServerApplication::onRender()
	{
		m_Renderer.waitForFences();

		onRenderUI();

		m_Renderer.recordUIData(ui::UIBackend::get_ui_render_data());

		m_Renderer.renderFrame();
	}

	void ServerApplication::onRenderUI()
	{
		ui::UIBackend::begin_frame();

		m_ClientsLayer.render();

		ui::UIBackend::end_frame();
	}

	void ServerApplication::initLayers()
	{

	}

	void ServerApplication::onUpdateConnectedClients()
	{
		while (auto pendingUsername = m_Server.getPendingUsernames().tryPop())
		{
			m_ClientsLayer.getClientsPanel().addUsername(*pendingUsername);

			m_ClientsLayer.getClientLogger().logClientConnected(*pendingUsername);
		}
	}

	void ServerApplication::onUpdateDisconnectedClients()
	{
		while (auto disconnectedUsername = m_Server.getDisconnectedUsernames().tryPop())
		{
			m_ClientsLayer.getClientsPanel().removeUsername(*disconnectedUsername);

			m_ClientsLayer.getClientLogger().logClientDisconnected(*disconnectedUsername);
		}
	}
}
