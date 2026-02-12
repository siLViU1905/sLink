#ifndef SLINK_UISERVERPORTLAYER_H
#define SLINK_UISERVERPORTLAYER_H

#include <layers/UILayer.h>

#include "../../components/port_select/UIPortSelect.h"

namespace sLink::server::ui::layer
{
    class UIServerPortLayer : public sLink::ui::layer::UILayer
    {
    public:
        void render() override;

        component::UIPortSelect& getPortSelectPanel();

    private:
        component::UIPortSelect m_PortSelect;
    };
}

#endif //SLINK_UISERVERPORTLAYER_H
