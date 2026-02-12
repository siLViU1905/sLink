#ifndef SLINK_UILOGINLAYER_H
#define SLINK_UILOGINLAYER_H
#include "../UILayer.h"

#include "components/client_login/UIClientLogin.h"

namespace sLink::ui::layer
{
    class UILoginLayer : public UILayer
    {
    public:
        void render() override;

        component::UIClientLogin& getClientLoginPanel();

    private:
        component::UIClientLogin m_ClientLoginPanel;
    };
}

#endif //SLINK_UILOGINLAYER_H
