#include "UiClientsLayer.h"

namespace sLink::server::ui::layer
{
    void UIClientsLayer::render()
    {
        m_ClientsPanel.render();

        m_ClientLogger.render();
    }

    component::UIActiveClients & UIClientsLayer::getClientsPanel()
    {
        return m_ClientsPanel;
    }

    component::UIClientLogging & UIClientsLayer::getClientLogger()
    {
        return m_ClientLogger;
    }
}
