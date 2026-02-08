#include "UiClientsLayer.h"

namespace sLink::ui::layer
{
    void UIClientsLayer::render()
    {
        m_ClientsPanel.render();
    }

    component::UIActiveClients & UIClientsLayer::getClientsPanel()
    {
        return m_ClientsPanel;
    }
}
