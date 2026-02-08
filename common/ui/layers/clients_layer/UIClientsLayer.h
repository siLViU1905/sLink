#ifndef SLINK_UICLIENTSLAYER_H
#define SLINK_UICLIENTSLAYER_H

#include "components/active_clients/UIActiveClients.h"
#include "layers/UILayer.h"

namespace sLink::ui::layer
{
    class UIClientsLayer : public UILayer
    {
    public:
        void render() override;

        component::UIActiveClients& getClientsPanel();

    private:
        component::UIActiveClients m_ClientsPanel;
    };
}

#endif
