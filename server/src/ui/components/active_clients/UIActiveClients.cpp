#include "UIActiveClients.h"

#include <print>

namespace sLink::server::ui::component
{
    UIActiveClients::UIActiveClients():m_ShowKickReasonPopup(false)
    {
        m_KickReason.resize(128, '\0');
    }

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
                {
                    m_UserToKick = username;

                    m_ShowKickReasonPopup = true;
                }

                ImGui::EndPopup();
            }

            if (ImGui::IsItemHovered())
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

            ImGui::Dummy(ImVec2(0.0f, s_ItemSpacing));

            ++index;
        }

        if (m_ShowKickReasonPopup)
            ImGui::OpenPopup("Kick Reason");

        if (ImGui::BeginPopupModal("Kick Reason", &m_ShowKickReasonPopup, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Reason for kicking ");
            ImGui::SameLine();
            ImGui::TextColored(s_ColorUsernameKickReason, "%s:", m_UserToKick.c_str());

            ImGui::Spacing();

            ImGui::InputText("##reason", m_KickReason.data(), m_KickReason.size());

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                m_ShowKickReasonPopup = false;

                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("Confirm Kick", ImVec2(120, 0)))
            {
                if (m_OnActionCallback)
                {
                    auto reason = m_KickReason.substr(0, m_KickReason.find_first_of('\0'));

                    m_OnActionCallback(Action::KICK, m_UserToKick, reason);
                }
                m_ShowKickReasonPopup = false;

                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
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
