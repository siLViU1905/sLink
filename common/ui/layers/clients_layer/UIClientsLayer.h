#ifndef SLINK_UICLIENTSLAYER_H
#define SLINK_UICLIENTSLAYER_H

#include "components/active_clients/UIActiveClients.h"
#include "components/client_logging/UIClientLogging.h"
#include "layers/UILayer.h"

namespace sLink::ui::layer
{
    class UIClientsLayer : public UILayer
    {
    public:
        void render() override;

        component::UIActiveClients& getClientsPanel();

        component::UIClientLogging& getClientLogger();

    private:
        component::UIActiveClients m_ClientsPanel;

        component::UIClientLogging m_ClientLogger;
    };
}

#endif
