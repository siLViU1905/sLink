#include "UIActiveClients.h"

#include <print>

namespace sLink::server::ui::component
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

        int index = 0;

        for (const auto &username: m_Usernames)
        {
            ImGui::Selectable(username.c_str());

            if (ImGui::BeginPopupContextItem())
            {
                ImGui::Text("Actions: %s", username.c_str());
                ImGui::Separator();

                if (ImGui::Selectable("Kick User"))
                    if (m_OnActionCallback)
                        m_OnActionCallback(Action::KICK, username);


                ImGui::EndPopup();
            }

            if (ImGui::IsItemHovered())
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

            ImGui::Dummy(ImVec2(0.0f, s_ItemSpacing));

            ++index;
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

    void UIActiveClients::setOnActionCallback(OnActionCallback &&callback)
    {
        m_OnActionCallback = std::move(callback);
    }
}
