#include "UIInfo.h"

namespace sLink::ui::component
{
    void UIInfo::render()
    {
        float window_height = ImGui::GetIO().DisplaySize.y;
        float half_height = window_height / 2.f;

        ImGui::SetNextWindowPos(ImVec2(s_SidebarWidth, half_height));
        ImGui::SetNextWindowSize(ImVec2(s_SidebarWidth, half_height));

        constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
                                                  ImGuiWindowFlags_NoResize |
                                                  ImGuiWindowFlags_NoMove |
                                                  ImGuiWindowFlags_NoCollapse |
                                                  ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::Begin("Info Console", nullptr, window_flags);
        ImGui::SetWindowFontScale(s_FontScale);

        ImGui::TextColored(s_ColorHeader, "INFO CONSOLE");
        ImGui::Separator();

        ImGui::BeginChild("InfoScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        for (const auto &info: m_Infos)
        {
            ImVec4 color;
            std::string prefix;

            switch (info.m_Type)
            {
                case Info::Type::SUCCESS:
                    color = s_ColorSuccess;
                    prefix = "[OK] ";
                    break;
                case Info::Type::FAIL:
                    color = s_ColorFail;
                    prefix = "[ERR] ";
                    break;
                case Info::Type::GENERAL:
                    color = s_ColorGeneral;
                    prefix = "[INFO] ";
                    break;
            }

            ImGui::TextColored(color, "%s%s", prefix.c_str(), info.m_Content.c_str());
        }

        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);

        ImGui::EndChild();
        ImGui::End();
    }

    void UIInfo::addSuccessInfo(std::string_view content)
    {
        m_Infos.emplace_back(content.data(), Info::Type::SUCCESS);
    }

    void UIInfo::addFailInfo(std::string_view content)
    {
        m_Infos.emplace_back(content.data(), Info::Type::FAIL);
    }

    void UIInfo::addGeneralInfo(std::string_view content)
    {
        m_Infos.emplace_back(content.data(), Info::Type::GENERAL);
    }

    const std::vector<UIInfo::Info> & UIInfo::getInfos() const
    {
        return m_Infos;
    }
}
