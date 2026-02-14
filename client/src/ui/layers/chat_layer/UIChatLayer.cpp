#include "UIChatLayer.h"

namespace sLink::client::ui::layer
{
	UIChatLayer::UIChatLayer():m_ChatWindow(m_InfoPanel.getInfos())
	{
	}

	void UIChatLayer::render()
	{
		m_ChatWindow.render();

		m_InfoPanel.render();
	}

	sLink::ui::component::UIInfo & UIChatLayer::getInfoPanel()
	{
		return m_InfoPanel;
	}

	component::UIChatWindow& UIChatLayer::getChatWindow()
	{
		return m_ChatWindow;
	}
}
