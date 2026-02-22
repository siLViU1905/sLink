#ifndef SLINK_UICLIENTREGISTER_H
#define SLINK_UICLIENTREGISTER_H

#include <components/UIComponent.h>

#include <functional>
#include <string>
#include <string_view>
#include <imgui.h>

namespace sLink::client::ui::component
{
    class UIClientRegister : public sLink::ui::component::UIComponent
    {
    private:
        static constexpr float s_WindowWidth = 350.f;

        static constexpr float s_WindowHeight = 350.f;

        static constexpr float s_InputPaddingY = 8.f;

        static constexpr float s_ItemSpacing = 15.0f;

        static constexpr ImVec4 s_ColorLink = { 0.2f, 0.6f, 1.0f, 1.0f };

    public:
        using OnRegisterDataInputCallback = std::move_only_function<void(std::string_view, std::string_view, std::string_view)>;

        using OnLoginClickedCallback = std::move_only_function<void()>;

        UIClientRegister();

        void render() override;

        void setOnRegisterDataInput(OnRegisterDataInputCallback &&callback);

        void setOnLoginClick(OnLoginClickedCallback &&callback);

        void notifyRegisterFailed(std::string_view message);

    private:
        std::string m_InputUsername;

        std::string m_InputPassword;

        std::string m_InputConfirmPassword;

        std::string m_InputServerPort;

        OnRegisterDataInputCallback m_OnRegisterDataInputCallback;

        OnLoginClickedCallback m_OnLoginClickCallback;

        bool m_ShowAuthIncorrectInfoErrorPopup;

        bool m_ShowRegisterFailedPopup;

        std::string m_AuthInfoErrorMessage;

        std::string m_RegisterFailMessage;
    };
}

#endif
