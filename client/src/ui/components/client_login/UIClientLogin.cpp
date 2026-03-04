#include "UIClientLogin.h"

namespace sLink::client::ui::component
{
    UIClientLogin::UIClientLogin() : m_ShowAuthIncorrectInfoErrorPopup(false), m_ShowLoginFailedPopup(false),
                                     m_ShowKickedPopup(false)
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

        float circleDiameter = s_ProfileCircleRadius * 2.f;
        float startPosX = (s_WindowWidth - circleDiameter) * 0.5f;
        ImVec2 profilePos = ImGui::GetCursorScreenPos();
        profilePos.x += startPosX - ImGui::GetStyle().WindowPadding.x;
        profilePos.y += 10.0f;

        drawCircularProfile(profilePos, circleDiameter, "LOAD");

        ImGui::SetCursorScreenPos(profilePos);
        if (ImGui::InvisibleButton("##profileCircle", ImVec2(circleDiameter, circleDiameter)))
        {
            if (m_OnLoadProfilePictureCallback)
                m_OnLoadProfilePictureCallback();
        }

        if (ImGui::IsItemHovered())
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20.0f);

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

        ImGui::Spacing();

        float text_width = ImGui::CalcTextSize("Don't have an account? Register").x;
        ImGui::SetCursorPosX((s_WindowWidth - text_width) * 0.5f);

        ImGui::PushStyleColor(ImGuiCol_Text, s_ColorLink);
        if (ImGui::Selectable("Don't have an account? Register", false, 0, ImVec2(text_width, 0)))
            if (m_OnRegisterClickCallback)
                m_OnRegisterClickCallback();

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

        if (m_ShowKickedPopup)
            ImGui::OpenPopup("Kicked");

        if (ImGui::BeginPopupModal("Kicked", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("%s", m_KickReason.c_str());
            ImGui::Separator();

            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                m_ShowKickedPopup = false;
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

    void UIClientLogin::setOnRegisterClick(OnRegisterClickCallback &&callback)
    {
        m_OnRegisterClickCallback = std::move(callback);
    }

    void UIClientLogin::setOnLoadProfilePicture(OnLoadProfilePictureCallback &&callback)
    {
        m_OnLoadProfilePictureCallback = std::move(callback);
    }

    void UIClientLogin::setTextureID(ProfilePictureTextureID id)
    {
        m_ProfilePictureTextureID = id;
    }

    void UIClientLogin::notifyLoginFailed(std::string_view message)
    {
        m_LoginFailMessage = message;

        m_ShowLoginFailedPopup = true;
    }

    void UIClientLogin::notifyKick(std::string_view reason)
    {
        m_KickReason = reason;

        m_ShowKickedPopup = true;
    }

    void UIClientLogin::drawCircularProfile(ImVec2 pos, float size, std::string_view placeholderText)
    {
        ImDrawList *drawList = ImGui::GetWindowDrawList();
        float rounding = size * 0.5f;
        ImVec2 center = ImVec2(pos.x + rounding, pos.y + rounding);

        if (m_ProfilePictureTextureID == 0)
        {
            drawList->AddCircleFilled(center, rounding, ImColor(60, 60, 60, 255), 64);

            ImVec2 textSize = ImGui::CalcTextSize(placeholderText.data());
            drawList->AddText(ImVec2(center.x - textSize.x * 0.5f, center.y - textSize.y * 0.5f),
                              ImColor(200, 200, 200, 255), placeholderText.data());
        }
        else
        {
            ImVec2 maxPos = ImVec2(pos.x + size, pos.y + size);
            drawList->AddImageRounded(
                m_ProfilePictureTextureID,
                pos,
                maxPos,
                ImVec2(0, 0), ImVec2(1, 1),
                IM_COL32_WHITE,
                rounding
            );
        }

        drawList->AddCircle(center, rounding, IM_COL32(200, 200, 200, 255), 64, 2.0f);

        ImGui::Dummy(ImVec2(size, size));
    }
}
