#include "UIActiveClients.h"

namespace sLink::ui::component
{
    void UIActiveClients::render()
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(s_PanelWidth, ImGui::GetIO().DisplaySize.y));

        constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
                                                  ImGuiWindowFlags_NoResize |
                                                  ImGuiWindowFlags_NoMove |
                                                  ImGuiWindowFlags_NoCollapse |
                                                  ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::Begin("Active Clients", nullptr, window_flags);
        ImGui::SetWindowFontScale(s_FontScale);

        ImGui::TextColored(s_ColorHeader, "ACTIVE USERS");
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, s_ItemSpacing));

        for (const auto &username: m_Usernames)
        {
            ImGui::Bullet();

            ImGui::SameLine();

            ImGui::TextColored(s_ColorUser, "%s", username.c_str());

            ImGui::Dummy(ImVec2(0.0f, s_ItemSpacing));
        }

        ImGui::End();
    }

    void UIActiveClients::addUsername(std::string_view username)
    {
        m_Usernames.emplace_back(username);
    }

    void UIActiveClients::removeUsername(std::string_view username)
    {
        std::erase(m_Usernames, username);
    }
}
