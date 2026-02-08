#ifndef SLINK_UI_CHAT_LAYER_H
#define SLINK_UI_CHAT_LAYER_H

#include "../UILayer.h"
#include <components/chat_window/UIChatWindow.h>

namespace sLink::ui::layer
{
	class UIChatLayer : public UILayer
	{
	public:
		void render() override;

		component::UIChatWindow& getChatWindow();
	private:
		component::UIChatWindow m_ChatWindow;
	};
}

#endif
