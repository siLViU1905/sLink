#ifndef SLINK_UI_CHAT_LAYER_H
#define SLINK_UI_CHAT_LAYER_H

#include <layers/UILayer.h>
#include "../../components/chat_window/UIChatWindow.h"

namespace sLink::client::ui::layer
{
	class UIChatLayer : public sLink::ui::layer::UILayer
	{
	public:
		void render() override;

		component::UIChatWindow& getChatWindow();
	private:
		component::UIChatWindow m_ChatWindow;
	};
}

#endif
