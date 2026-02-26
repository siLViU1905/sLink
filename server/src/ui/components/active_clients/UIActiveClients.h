#ifndef SLINK_UIACTIVECLIENTS_H
#define SLINK_UIACTIVECLIENTS_H

#include <string>
#include <string_view>
#include <vector>

#include <components/UIComponent.h>
#include <imgui.h>
#include <functional>

namespace sLink::server::ui::component
{
    class UIActiveClients : public sLink::ui::component::UIComponent
    {
    public:
        static constexpr float s_PanelWidth = 200.f;

    private:
        static constexpr float s_FontScale = 1.15f;

        static constexpr ImVec4 s_ColorHeader = {0.2f, 0.7f, 0.9f, 1.0f};

        static constexpr ImVec4 s_ColorUser = {1.0f, 1.0f, 1.0f, 1.0f};

        static constexpr float s_ItemSpacing = 4.0f;

    public:
        enum class Action
        {
            KICK
        };

        using OnActionCallback = std::move_only_function<void(Action, const std::string&)>;

        void render() override;

        void addUsername(std::string_view username);

        void removeUsername(std::string_view username);

        void setOnActionCallback(OnActionCallback &&callback);

    private:
        std::vector<std::string> m_Usernames;

        OnActionCallback m_OnActionCallback;
    };
};

#endif
