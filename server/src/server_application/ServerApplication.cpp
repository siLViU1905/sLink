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

	void ServerApplication::onUpdate()
	{
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

		m_MainLayer.render();

		ui::UIBackend::end_frame();
	}

	void ServerApplication::initLayers()
	{
		
	}
}
