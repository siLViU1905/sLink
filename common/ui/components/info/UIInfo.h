#ifndef SLINK_UIINFO_H
#define SLINK_UIINFO_H
#include <string>
#include <vector>

#include "components/UIComponent.h"
#include <imgui.h>

namespace sLink::ui::component
{
    class UIInfo : public UIComponent
    {
    private:
        static constexpr float s_FontScale = 1.1f;
        static constexpr float s_SidebarWidth = 300.f;

        static constexpr ImVec4 s_ColorSuccess = { 0.2f, 0.8f, 0.2f, 1.f };
        static constexpr ImVec4 s_ColorFail    = { 1.f, 0.2f, 0.2f, 1.f };
        static constexpr ImVec4 s_ColorInfo    = { 0.2f, 0.6f, 1.f, 1.f };
        static constexpr ImVec4 s_ColorHeader  = { 0.8f, 0.8f, 0.f, 1.f };

    public:
        struct Info
        {
            enum class Type
            {
                SUCCESS,
                FAIL,
                GENERAL
            };

            std::string m_Content;

            Type m_Type;
        };

        void render() override;

        void addSuccessInfo(std::string_view content);

        void addFailInfo(std::string_view content);

        void addGeneralInfo(std::string_view content);

    private:
        std::vector<Info> m_Infos;
    };
}

#endif //SLINK_UIINFO_H
