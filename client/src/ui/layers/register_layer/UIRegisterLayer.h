#ifndef SLINK_UIREGISTERLAYER_H
#define SLINK_UIREGISTERLAYER_H

#include <layers/UILayer.h>
#include "../../components/client_register/UIClientRegister.h"

namespace sLink::client::ui::layer
{
    class UIRegisterLayer : public sLink::ui::layer::UILayer
    {
    public:
        void render() override;

        component::UIClientRegister &getClientRegisterPanel();

    private:
        component::UIClientRegister m_ClientRegisterPanel;
    };
}

#endif