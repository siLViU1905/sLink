#ifndef SLINK_UI_CHAT_LAYER_H
#define SLINK_UI_CHAT_LAYER_H

#include <layers/UILayer.h>
#include "../../components/chat_window/UIChatWindow.h"
#include <components/info/UIInfo.h>

namespace sLink::client::ui::layer
{
	class UIChatLayer : public sLink::ui::layer::UILayer
	{
	public:
		UIChatLayer();

		void render() override;

		sLink::ui::component::UIInfo& getInfoPanel();

		component::UIChatWindow& getChatWindow();

	private:
		sLink::ui::component::UIInfo m_InfoPanel;

		component::UIChatWindow m_ChatWindow;
	};
}

#endif
