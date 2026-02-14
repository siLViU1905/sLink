#include "UIChatLayer.h"

namespace sLink::client::ui::layer
{
	void UIChatLayer::render()
	{
		m_ChatWindow.render();

		m_InfoPanel.render();
	}

	component::UIChatWindow& UIChatLayer::getChatWindow()
	{
		return m_ChatWindow;
	}

	sLink::ui::component::UIInfo & UIChatLayer::getInfoPanel()
	{
		return m_InfoPanel;
	}
}
