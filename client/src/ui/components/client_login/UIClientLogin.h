#ifndef SLINK_UICLIENTLOGIN_H
#define SLINK_UICLIENTLOGIN_H
#include <string>

#include "asio/detail/executor_function.hpp"
#include <components/UIComponent.h>
#include <imgui.h>

namespace sLink::client::ui::component
{
    class UIClientLogin : public sLink::ui::component::UIComponent
    {
    private:
        static constexpr float s_WindowWidth = 350.f;

        static constexpr float s_WindowHeight = 300.f;

        static constexpr float s_InputPaddingY = 8.f;

        static constexpr float s_ItemSpacing = 15.0f;

        static constexpr ImVec4 s_ColorLink = { 0.2f, 0.6f, 1.0f, 1.0f };

    public:
        using OnLoginDataInputCallback = std::move_only_function<void(std::string_view, std::string_view,
                                                                      std::string_view)>;

        using OnRegisterClickCallback = std::move_only_function<void()>;

        UIClientLogin();

        void render() override;

        void setOnLoginDataInput(OnLoginDataInputCallback &&callback);

        void setOnRegisterClick(OnRegisterClickCallback &&callback);

        void notifyLoginFailed(std::string_view message);

    private:
        std::string m_InputUsername;

        std::string m_InputPassword;

        std::string m_InputServerPort;

        OnLoginDataInputCallback m_OnLoginDataInputCallback;

        OnRegisterClickCallback m_OnRegisterClickCallback;

        bool m_ShowAuthIncorrectInfoErrorPopup;

        bool m_ShowLoginFailedPopup;

        std::string m_AuthInfoErrorMessage;

        std::string m_LoginFailMessage;
    };
}

#endif //SLINK_UICLIENTLOGIN_H
