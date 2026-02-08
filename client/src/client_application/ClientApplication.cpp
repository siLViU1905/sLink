#include "ClientApplication.h"

#include<components/chat_window/UIChatWindow.h>

namespace sLink::client_application
{
	ClientApplication::ClientApplication(int windowWidth, int windowHeight, std::string_view windowName):
		application::Application(windowWidth, windowHeight, windowName),
		m_Client(m_IOContext)
	{
		initLayers();

		m_NetworkThread = std::jthread([this]()
			{
				m_IOContext.run();
			});

		m_Client.setUsername("ClientTest");

		m_Client.connect("127.0.0.1", "12444");
	}

	ClientApplication::~ClientApplication()
	{
		m_IOContext.stop();
	}

	void ClientApplication::onUpdate()
	{
		while (auto raw_message = m_Client.getInbox().tryPop())
		{

		}
	}

	void ClientApplication::onRender()
	{
		m_Renderer.waitForFences();

		onRenderUI();

		m_Renderer.recordUIData(ui::UIBackend::get_ui_render_data());

		m_Renderer.renderFrame();
	}

	void ClientApplication::onRenderUI()
	{
		ui::UIBackend::begin_frame();

		m_MainLayer.render();

		ui::UIBackend::end_frame();
	}

	void ClientApplication::initLayers()
	{
		auto component = std::make_unique<ui::component::UIChatWindow>();

		component->setOnMessageSend([this](std::string_view content)
			{
				message::Message message(
					m_Client.getUsername(),
					content
				);

				m_Client.send(message);
			});

		m_MainLayer.addComponent(std::move(component));
	}
}
