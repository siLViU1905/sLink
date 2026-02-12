#include "UIClientLogin.h"

#include <imgui.h>

namespace sLink::ui::component
{
    UIClientLogin::UIClientLogin()
    {
        m_InputUsername.reserve(25);

        m_InputServerPort.reserve(5);
    }

    void UIClientLogin::render()
    {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(s_WindowWidth, s_WindowHeight));

        ImGui::Begin("Login to sLink", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, s_InputPaddingY));

        ImGui::Text("Username:");
        ImGui::InputText("##username", m_InputUsername.data(), 26);

        ImGui::Dummy(ImVec2(0, s_ItemSpacing));

        ImGui::Text("Server Port:");
        ImGui::InputText("##port", m_InputServerPort.data(), 6, ImGuiInputTextFlags_CharsDecimal);

        ImGui::PopStyleVar();
        ImGui::Dummy(ImVec2(0, s_ItemSpacing));

        if (ImGui::Button("Connect", ImVec2(-1, 40)))
        {
            std::string userStr(m_InputUsername.c_str());
            std::string portStr(m_InputServerPort.c_str());

            if (userStr.empty())
            {
                m_ErrorMessage = "Username field is empty!";
                m_ShowErrorPopup = true;
            }
            else if (portStr.empty())
            {
                m_ErrorMessage = "Server Port field is empty!";
                m_ShowErrorPopup = true;
            }
            else if (m_OnLoginDataInputCallback)
            {
                m_OnLoginDataInputCallback(userStr, portStr);
            }
        }

        if (m_ShowErrorPopup)
        {
            ImGui::OpenPopup("Login Error");
        }

        if (ImGui::BeginPopupModal("Login Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("%s", m_ErrorMessage.c_str());
            ImGui::Separator();

            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                m_ShowErrorPopup = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::End();
    }

    void UIClientLogin::setOnLoginDataInput(OnLoginDataInputCallback &&callback)
    {
        m_OnLoginDataInputCallback = std::move(callback);
    }
}
