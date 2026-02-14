#ifndef SLINK_UI_CHAT_WINDOW_H
#define SLINK_UI_CHAT_WINDOW_H

#include <components/UIComponent.h>
#include <message/Message.h>
#include <vector>
#include <functional>
#include <imgui.h>
#include <components/info/UIInfo.h>

namespace sLink::client::ui::component
{
    class UIChatWindow: public sLink::ui::component::UIComponent
    {
    private:
        static constexpr float s_FontScale = 2.15f;
        static constexpr float s_MessageSpacing = 6.f;

        static constexpr float s_InputPaddingY = 10.f;
        static constexpr float s_ButtonWidth = 80.f;
        static constexpr float s_ButtonHeight = 40.f;
        static constexpr float s_ButtonRounding = 8.0;

        static constexpr ImVec4 s_ColorTimestamp = { 0.6f, 0.6f, 0.6f, 1.0f };
        static constexpr ImVec4 s_ColorUsername  = { 0.2f, 0.7f, 0.9f, 1.0f };
        static constexpr ImVec4 s_ColorContent   = { 1.0f, 1.0f, 1.0f, 1.0f };

        static constexpr ImVec4 s_ColorButton        = { 0.15f, 0.45f, 0.85f, 1.f };
        static constexpr ImVec4 s_ColorButtonHovered = { 0.2f, 0.55f, 0.95f, 1.f };
        static constexpr ImVec4 s_ColorButtonActive  = { 0.1f, 0.35f, 0.75f, 1.f };

        static constexpr ImVec4 s_ColorInfoSuccess = sLink::ui::component::UIInfo::s_ColorSuccess;
        static constexpr ImVec4 s_ColorInfoFail    = sLink::ui::component::UIInfo::s_ColorFail;
        static constexpr ImVec4 s_ColorInfoGeneral = sLink::ui::component::UIInfo::s_ColorInfo;

    public:
        using OnSendCallback = std::move_only_function<void(std::string_view)>;

        UIChatWindow(const std::vector<sLink::ui::component::UIInfo::Info> &infos);

        void render() override;

        void addMessage(const message::Message& message);

        void setOnMessageSend(OnSendCallback&& callback);

    private:
        std::vector<message::Message> m_Messages;

        const std::vector<sLink::ui::component::UIInfo::Info>& m_InfosRef;

        std::string m_InputContent;

        OnSendCallback m_OnSendCallback;
    };
}
#endif
