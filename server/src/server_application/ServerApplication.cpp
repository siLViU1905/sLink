#include "ServerApplication.h"

namespace sLink::server_application
{
    ServerApplication::ServerApplication(int windowWidth, int windowHeight, std::string_view windowName)
        : Application(windowWidth, windowHeight, windowName), m_Server(m_IOContext)
    {
        initLayers();

        m_NetworkThread = std::jthread([this]()
        {
            m_IOContext.run();
        });

        m_DbThread = std::jthread([this]()
        {
            m_Database.run(m_Server.getDbUsernameInbox());
        });
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

        onUpdateDbInfo();
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

        m_CurrentLayer->render();

        ui::UIBackend::end_frame();
    }

    void ServerApplication::initLayers()
    {
        m_ClientsLayer = std::make_shared<server::ui::layer::UIClientsLayer>();

        m_ServerPortLayer = std::make_shared<server::ui::layer::UIServerPortLayer>();

        m_ServerPortLayer->getPortSelectPanel().setOnPortInput([this](std::string_view port)
        {
            onPortSelected(port);
        });

        m_CurrentLayer = m_ServerPortLayer;
    }

    void ServerApplication::onPortSelected(std::string_view port)
    {
        uint16_t portNumber = 0;

        std::from_chars(port.data(), port.data() + port.size(), portNumber);

        auto result = m_Server.startHost(portNumber);

        if (result)
            m_ClientsLayer->getInfoPanel().addSuccessInfo(*result);
        else
            m_ClientsLayer->getInfoPanel().addFailInfo(*result);

        m_CurrentLayer = m_ClientsLayer;
    }

    void ServerApplication::onUpdateConnectedClients()
    {
        while (auto pendingUsername = m_Server.getPendingUsernames().tryPop())
        {
            m_ClientsLayer->getClientsPanel().addUsername(*pendingUsername);

            m_ClientsLayer->getClientLogger().logClientConnected(*pendingUsername);
        }
    }

    void ServerApplication::onUpdateDisconnectedClients()
    {
        while (auto disconnectedUsername = m_Server.getDisconnectedUsernames().tryPop())
        {
            m_ClientsLayer->getClientsPanel().removeUsername(*disconnectedUsername);

            m_ClientsLayer->getClientLogger().logClientDisconnected(*disconnectedUsername);
        }
    }

    void ServerApplication::onUpdateDbInfo()
    {
        while (auto info = m_Database.getInfo().tryPop())
            m_ClientsLayer->getInfoPanel().addGeneralInfo("[DB] " + *info);
    }
}
