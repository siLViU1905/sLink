#ifndef SLINK_UICLIENTSLAYER_H
#define SLINK_UICLIENTSLAYER_H

#include "../../components/active_clients/UIActiveClients.h"
#include "../../components/client_logging/UIClientLogging.h"
#include <layers/UILayer.h>

#include "components/info/UIInfo.h"

namespace sLink::server::ui::layer
{
    class UIClientsLayer : public sLink::ui::layer::UILayer
    {
    public:
        void render() override;

        component::UIActiveClients& getClientsPanel();

        component::UIClientLogging& getClientLogger();

        sLink::ui::component::UIInfo& getInfoPanel();

    private:
        component::UIActiveClients m_ClientsPanel;

        component::UIClientLogging m_ClientLogger;

        sLink::ui::component::UIInfo m_InfoPanel;
    };
}

#endif
