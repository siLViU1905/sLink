#include "UIRegisterLayer.h"

void sLink::client::ui::layer::UIRegisterLayer::render()
{
    m_ClientRegisterPanel.render();
}

sLink::client::ui::component::UIClientRegister & sLink::client::ui::layer::UIRegisterLayer::getClientRegisterPanel()
{
    return m_ClientRegisterPanel;
}
