#include "UIClientRegister.h"

namespace sLink::client::ui::component
{
    UIClientRegister::UIClientRegister() : m_ShowAuthIncorrectInfoErrorPopup(false), m_ShowRegisterFailedPopup(false)
    {
        m_InputUsername.resize(25);

        m_InputPassword.resize(32);

        m_InputConfirmPassword.resize(32);

        m_InputServerPort.resize(5);
    }

    void UIClientRegister::render()
    {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(s_WindowWidth, s_WindowHeight));

        ImGui::Begin("Register to sLink", nullptr,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, s_InputPaddingY));

        ImGui::Text("Username:");
        ImGui::InputText("##username", m_InputUsername.data(), 26);

        ImGui::Text("Password:");
        ImGui::InputText("##password", m_InputPassword.data(), 33, ImGuiInputTextFlags_Password);

        ImGui::Text("Confirm Password:");
        ImGui::InputText("##confirm_password", m_InputConfirmPassword.data(), 33, ImGuiInputTextFlags_Password);

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

            auto confirmPassword = m_InputConfirmPassword.substr(0, m_InputConfirmPassword.find_first_of('\0'));

            auto serverPort = m_InputServerPort.substr(0, m_InputServerPort.find_first_of('\0'));

            if (username.empty())
            {
                m_AuthInfoErrorMessage = "Username field is empty!";
                m_ShowAuthIncorrectInfoErrorPopup = true;
            } else if (password.empty())
            {
                m_AuthInfoErrorMessage = "Password field is empty!";
                m_ShowAuthIncorrectInfoErrorPopup = true;
            } else if (confirmPassword.empty())
            {
                m_AuthInfoErrorMessage = "Confirm Password field is empty!";
                m_ShowAuthIncorrectInfoErrorPopup = true;
            }
            else if (password != confirmPassword)
            {
                m_AuthInfoErrorMessage = "Password and confirmed password does not match!";
                m_ShowAuthIncorrectInfoErrorPopup = true;
            }
            else if (serverPort.empty())
            {
                m_AuthInfoErrorMessage = "Server Port field is empty!";
                m_ShowAuthIncorrectInfoErrorPopup = true;
            } else if (m_OnRegisterDataInputCallback)
                m_OnRegisterDataInputCallback(username, password, serverPort);
        }

        ImGui::Spacing();

        float text_width = ImGui::CalcTextSize("Have an account? Login").x;
        ImGui::SetCursorPosX((s_WindowWidth - text_width) * 0.5f);

        ImGui::PushStyleColor(ImGuiCol_Text, s_ColorLink);
        if (ImGui::Selectable("Have an account? Login", false, 0, ImVec2(text_width, 0)))
            if (m_OnLoginClickCallback)
                m_OnLoginClickCallback();

        ImGui::PopStyleColor();

        if (ImGui::IsItemHovered())
        {
            ImVec2 min = ImGui::GetItemRectMin();
            ImVec2 max = ImGui::GetItemRectMax();
            min.y = max.y;
            ImGui::GetWindowDrawList()->AddLine(min, max, ImGui::ColorConvertFloat4ToU32(s_ColorLink));
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
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


        if (m_ShowRegisterFailedPopup)
            ImGui::OpenPopup("Register Fail Error");

        if (ImGui::BeginPopupModal("Register Fail Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("%s", m_RegisterFailMessage.c_str());
            ImGui::Separator();

            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                m_ShowRegisterFailedPopup = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::End();
    }

    void UIClientRegister::setOnRegisterDataInput(OnRegisterDataInputCallback &&callback)
    {
        m_OnRegisterDataInputCallback = std::move(callback);
    }

    void UIClientRegister::setOnLoginClick(OnLoginClickedCallback &&callback)
    {
        m_OnLoginClickCallback = std::move(callback);
    }

    void UIClientRegister::notifyRegisterFailed(std::string_view message)
    {
        m_RegisterFailMessage = message;

        m_ShowRegisterFailedPopup = true;
    }
}
