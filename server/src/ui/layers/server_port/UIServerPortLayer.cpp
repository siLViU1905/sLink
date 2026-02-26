#include "UIServerPortLayer.h"

namespace sLink::server::ui::layer
{
    void UIServerPortLayer::render()
    {
        m_PortSelect.render();
    }

    component::UIPortSelect &UIServerPortLayer::getPortSelectPanel()
    {
        return m_PortSelect;
    }
}