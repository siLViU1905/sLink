#ifndef SLINK_UI_CHAT_WINDOW_H
#define SLINK_UI_CHAT_WINDOW_H

#include <components/UIComponent.h>
#include <message/Message.h>
#include <vector>
#include <functional>

namespace sLink::ui::component
{
    class UIChatWindow: public UIComponent
    {
    public:
        using OnSendCallback = std::move_only_function<void(std::string_view)>;

        UIChatWindow();

        void render() override;

        void addMessage(const message::Message& message);

        void setOnMessageSend(OnSendCallback&& callback);

    private:
        std::vector<message::Message> m_Messages;

        std::string m_InputContent;

        OnSendCallback m_OnSendCallback;
    };
}
#endif
