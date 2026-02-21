#include "UIClientLogin.h"

#include <imgui.h>

namespace sLink::client::ui::component
{
    UIClientLogin::UIClientLogin() : m_ShowAuthIncorrectInfoErrorPopup(false), m_ShowLoginFailedPopup(false)
    {
        m_InputUsername.resize(25);

        m_InputPassword.resize(32);

        m_InputServerPort.resize(5);
    }

    void UIClientLogin::render()
    {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(s_WindowWidth, s_WindowHeight));

        ImGui::Begin("Login to sLink", nullptr,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, s_InputPaddingY));

        ImGui::Text("Username:");
        ImGui::InputText("##username", m_InputUsername.data(), 26);

        ImGui::Text("Password:");
        ImGui::InputText("##password", m_InputPassword.data(), 33, ImGuiInputTextFlags_Password);
        ImGui::Dummy(ImVec2(0, 5));

        ImGui::Dummy(ImVec2(0, s_ItemSpacing));

        ImGui::Text("Server Port:");
        ImGui::InputText("##port", m_InputServerPort.data(), 6, ImGuiInputTextFlags_CharsDecimal);

        ImGui::PopStyleVar();
        ImGui::Dummy(ImVec2(0, s_ItemSpacing));

        if (ImGui::Button("Connect", ImVec2(-1, 40)))
        {
            auto username = m_InputUsername.substr(0, m_InputUsername.find_first_of('\0'));

            auto password = m_InputPassword.substr(0, m_InputPassword.find_first_of('\0'));

            auto serverPort = m_InputServerPort.substr(0, m_InputServerPort.find_first_of('\0'));

            if (username.empty())
            {
                m_AuthInfoErrorMessage = "Username field is empty!";
                m_ShowAuthIncorrectInfoErrorPopup = true;
            } else if (password.empty())
            {
                m_AuthInfoErrorMessage = "Password field is empty!";
                m_ShowAuthIncorrectInfoErrorPopup = true;
            } else if (serverPort.empty())
            {
                m_AuthInfoErrorMessage = "Server Port field is empty!";
                m_ShowAuthIncorrectInfoErrorPopup = true;
            } else if (m_OnLoginDataInputCallback)
                m_OnLoginDataInputCallback(username, password, serverPort);
        }

        if (m_ShowAuthIncorrectInfoErrorPopup)
            ImGui::OpenPopup("Auth Info Error");

        if (ImGui::BeginPopupModal("Auth Info Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("%s", m_AuthInfoErrorMessage.c_str());
            ImGui::Separator();

            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                m_ShowAuthIncorrectInfoErrorPopup = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }


        if (m_ShowLoginFailedPopup)
            ImGui::OpenPopup("Login Fail Error");

        if (ImGui::BeginPopupModal("Login Fail Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("%s", m_LoginFailMessage.c_str());
            ImGui::Separator();

            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                m_ShowLoginFailedPopup = false;
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

    void UIClientLogin::notifyLoginFailed(std::string_view message)
    {
        m_LoginFailMessage = message;

        m_ShowLoginFailedPopup = true;
    }
}
