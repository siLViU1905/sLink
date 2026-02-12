#ifndef SLINK_UILOGINLAYER_H
#define SLINK_UILOGINLAYER_H

#include <layers/UILayer.h>

#include "../../components/client_login/UIClientLogin.h"

namespace sLink::client::ui::layer
{
    class UILoginLayer : public sLink::ui::layer::UILayer
    {
    public:
        void render() override;

        component::UIClientLogin& getClientLoginPanel();

    private:
        component::UIClientLogin m_ClientLoginPanel;
    };
}

#endif //SLINK_UILOGINLAYER_H
