#ifndef SLINK_UIPORTSELECT_H
#define SLINK_UIPORTSELECT_H

#include <components/UIComponent.h>

#include <functional>
#include <string>
#include <string_view>

namespace sLink::server::ui::component
{
    class UIPortSelect : public sLink::ui::component::UIComponent
    {
    private:
        static constexpr float s_WindowWidth = 350.f;

        static constexpr float s_WindowHeight = 220.f;

        static constexpr float s_InputPaddingY = 8.f;

        static constexpr float s_ItemSpacing = 15.0f;

    public:
        using OnPortInputCallback = std::move_only_function<void(std::string_view)>;

        UIPortSelect();

        void render() override;

        void setOnPortInput(OnPortInputCallback &&callback);

    private:
        std::string m_InputPort;

        OnPortInputCallback m_OnPortInputCallback;

        bool m_ShowErrorPopup;

        std::string m_ErrorMessage;
    };
}

#endif //SLINK_UIPORTSELECT_H