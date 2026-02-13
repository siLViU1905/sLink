#include "UiClientsLayer.h"

namespace sLink::server::ui::layer
{
    void UIClientsLayer::render()
    {
        m_ClientsPanel.render();

        m_ClientLogger.render();

        m_InfoPanel.render();
    }

    component::UIActiveClients & UIClientsLayer::getClientsPanel()
    {
        return m_ClientsPanel;
    }

    component::UIClientLogging & UIClientsLayer::getClientLogger()
    {
        return m_ClientLogger;
    }

    sLink::ui::component::UIInfo & UIClientsLayer::getInfoPanel()
    {
        return m_InfoPanel;
    }
}
