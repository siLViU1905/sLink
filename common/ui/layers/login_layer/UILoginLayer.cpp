#include "UILoginLayer.h"

namespace sLink::ui::layer
{
    void UILoginLayer::render()
    {
        m_ClientLoginPanel.render();
    }

    component::UIClientLogin & UILoginLayer::getClientLoginPanel()
    {
        return m_ClientLoginPanel;
    }
}
