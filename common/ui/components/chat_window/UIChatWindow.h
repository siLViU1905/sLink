#ifndef SLINK_UI_CHAT_WINDOW_H
#define SLINK_UI_CHAT_WINDOW_H

#include <components/UIComponent.h>
#include <message/Message.h>
#include <vector>

namespace sLink::ui::component
{
    class UIChatWindow: public UIComponent
    {
    public:
        UIChatWindow();

        void render() override;

        void addMessage(const message::Message& message);
    private:
        std::vector<message::Message> m_Messages;

        std::string m_InputContent;
    };
}
#endif
