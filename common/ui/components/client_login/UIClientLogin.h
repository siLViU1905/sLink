#ifndef SLINK_UICLIENTLOGIN_H
#define SLINK_UICLIENTLOGIN_H
#include <string>

#include "asio/detail/executor_function.hpp"
#include "components/UIComponent.h"

namespace sLink::ui::component
{
    class UIClientLogin : public UIComponent
    {
    private:
        static constexpr float s_WindowWidth = 350.f;

        static constexpr float s_WindowHeight = 220.f;

        static constexpr float s_InputPaddingY = 8.f;

        static constexpr float s_ItemSpacing = 15.0f;

    public:
        using OnLoginDataInputCallback = std::move_only_function<void(std::string_view, std::string_view)>;

        UIClientLogin();

        void render() override;

        void setOnLoginDataInput(OnLoginDataInputCallback&& callback);

    private:
        std::string m_InputUsername;

        std::string m_InputServerPort;

        OnLoginDataInputCallback m_OnLoginDataInputCallback;

        bool m_ShowErrorPopup;

        std::string m_ErrorMessage;
    };
}

#endif //SLINK_UICLIENTLOGIN_H
