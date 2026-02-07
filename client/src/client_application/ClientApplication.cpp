#include "ClientApplication.h"

#include<components/chat_window/UIChatWindow.h>

namespace sLink::client_application
{
	ClientApplication::ClientApplication(int windowWidth, int windowHeight, std::string_view windowName):
		application::Application(windowWidth, windowHeight, windowName),
		m_Client(m_IOContext)
	{
		m_NetworkThread = std::jthread([this]()
			{
				m_IOContext.run();
			});

		m_MainLayer.addComponent(std::make_unique<ui::component::UIChatWindow>());
	}

	void ClientApplication::onUpdate()
	{
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
}