#include "ClientApplication.h"

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
	}

	ClientApplication::~ClientApplication()
	{
		m_IOContext.stop();
	}

	void ClientApplication::onUpdate()
	{
		while (auto raw_message = m_Client.getInbox().tryPop())
		{
			auto message = message::Message::deserialize(*raw_message);

			m_ChatLayer->getChatWindow().addMessage(message);
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

		m_CurrentLayer->render();

		ui::UIBackend::end_frame();
	}

	void ClientApplication::initLayers()
	{
		m_ChatLayer = std::make_shared<client::ui::layer::UIChatLayer>();

		m_ChatLayer->getChatWindow().setOnMessageSend([this](std::string_view content)
			{
				message::Message message(
					m_Client.getUsername(),
					content
				);

				m_Client.send(message);
			});


		m_LoginLayer = std::make_shared<client::ui::layer::UILoginLayer>();

		m_LoginLayer->getClientLoginPanel().setOnLoginDataInput([this](std::string_view username, std::string_view serverPort)
		{
			onConnect(username, serverPort);
		});

		m_CurrentLayer = m_LoginLayer;
	}

	void ClientApplication::onConnect(std::string_view username, std::string_view serverPort)
	{
		m_Client.setUsername(username);

		auto result = m_Client.connect("127.0.0.1", serverPort);

		if (result)
			m_ChatLayer->getInfoPanel().addSuccessInfo(*result);
		else
			m_ChatLayer->getInfoPanel().addFailInfo(result.error());

		m_CurrentLayer = m_ChatLayer;
	}
}
