#include "UIClientLogging.h"

#include "../active_clients/UIActiveClients.h"

namespace sLink::ui::component
{
    void UIClientLogging::render()
    {
        constexpr float offset_left = UIActiveClients::s_PanelWidth;

        ImGui::SetNextWindowPos(ImVec2(offset_left, 0));

        ImGui::SetNextWindowSize(ImVec2(s_PanelWidth, ImGui::GetIO().DisplaySize.y / 2.f));

        constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
                                                  ImGuiWindowFlags_NoResize |
                                                  ImGuiWindowFlags_NoMove |
                                                  ImGuiWindowFlags_NoCollapse |
                                                  ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::Begin("Client Logs", nullptr, window_flags);
        ImGui::SetWindowFontScale(s_FontScale);

        ImGui::TextColored(s_ColorHeader, "SYSTEM LOGS");
        ImGui::Separator();

        for (const auto &log: m_Logs)
        {
            if (log.m_State == ClientLog::State::CONNECTED)
                ImGui::TextColored(s_ColorConnected, "%s connected", log.m_Username.c_str());
            else
                ImGui::TextColored(s_ColorDisconnected, "%s disconnected", log.m_Username.c_str());
        }

        ImGui::End();
    }

    void UIClientLogging::logClientConnected(std::string_view username)
    {
        m_Logs.emplace_back(username.data(), ClientLog::State::CONNECTED);
    }

    void UIClientLogging::logClientDisconnected(std::string_view username)
    {
        m_Logs.emplace_back(username.data(), ClientLog::State::DISCONNECTED);
    }
}
