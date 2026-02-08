#include "UIChatLayer.h"

namespace sLink::ui::layer
{
	void UIChatLayer::render()
	{
		m_ChatWindow.render();
	}

	component::UIChatWindow& UIChatLayer::getChatWindow()
	{
		return m_ChatWindow;
	}
}
