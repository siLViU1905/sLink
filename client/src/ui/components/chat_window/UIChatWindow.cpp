#include "UIChatWindow.h"

#include <imgui.h>

namespace sLink::client::ui::component
{
    UIChatWindow::UIChatWindow(const std::vector<sLink::ui::component::UIInfo::Info> &infos)
        : m_InfosRef(infos),
          m_InputContent(255, '\0')
    {
    }

    void UIChatWindow::render()
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);

        constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
                                                  ImGuiWindowFlags_NoResize |
                                                  ImGuiWindowFlags_NoMove |
                                                  ImGuiWindowFlags_NoCollapse |
                                                  ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::Begin("Chat Window", nullptr, window_flags);

        ImGui::SetWindowFontScale(s_FontScale);

        float footer_height = s_ButtonHeight + ImGui::GetStyle().ItemSpacing.y * 2;

        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height), false);

        for (const auto &info: m_InfosRef)
        {
            ImVec4 infoColor;
            std::string tag;

            switch (info.m_Type)
            {
                case sLink::ui::component::UIInfo::Info::Type::SUCCESS:
                    infoColor = s_ColorInfoSuccess;
                    tag = "[SYSTEM-OK]";
                    break;
                case sLink::ui::component::UIInfo::Info::Type::FAIL:
                    infoColor = s_ColorInfoFail;
                    tag = "[SYSTEM-ERR]";
                    break;
                case sLink::ui::component::UIInfo::Info::Type::GENERAL:
                    infoColor = s_ColorInfoGeneral;
                    tag = "[SYSTEM-INFO]";
                    break;
            }

            ImGui::BeginGroup();
            ImGui::TextColored(infoColor, "%s %s", tag.c_str(), info.m_Content.c_str());
            ImGui::EndGroup();
            ImGui::Dummy(ImVec2(0.0f, s_MessageSpacing));
        }

        if (!m_InfosRef.empty() && !m_Messages.empty())
            ImGui::Separator();

        for (const auto &message: m_Messages)
        {
            ImGui::BeginGroup();

            ImGui::TextColored(s_ColorTimestamp, "[%s]", message.getTimestamp().toString().c_str());

            ImVec2 pos = ImGui::GetCursorScreenPos();
            float avatarSize = ImGui::GetFontSize() * 1.5f;
            drawCircularProfile(message.getSenderName(), pos, avatarSize, "IMG");
            ImGui::SameLine();

            ImGui::SameLine();
            ImGui::TextColored(s_ColorUsername, "%s:", message.getSenderName().data());

            ImGui::SameLine();
            ImGui::TextColored(s_ColorContent, "%s", message.getContent().data());

            ImGui::EndGroup();

            ImGui::Dummy(ImVec2(0.0f, s_MessageSpacing));
        }
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);

        ImGui::EndChild();

        ImGui::Separator();

        float spacing = ImGui::GetStyle().ItemSpacing.x;

        float available_width = ImGui::GetContentRegionAvail().x;

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, s_InputPaddingY));

        ImGui::PushItemWidth(available_width - s_ButtonWidth - spacing);

        bool input_submitted = ImGui::InputText("##input", m_InputContent.data(), 256,
                                                ImGuiInputTextFlags_EnterReturnsTrue);

        ImGui::PopItemWidth();
        ImGui::PopStyleVar();

        ImGui::SameLine();

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, s_ButtonRounding);
        ImGui::PushStyleColor(ImGuiCol_Button, s_ColorButton);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, s_ColorButtonHovered);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, s_ColorButtonActive);

        if (ImGui::Button("Send", ImVec2(s_ButtonWidth, s_ButtonHeight)) || input_submitted)
        {
            if (m_InputContent[0] != '\0')
            {
                if (m_OnSendCallback)
                {
                    auto clean_content = m_InputContent.substr(0, m_InputContent.find('\0'));
                    m_OnSendCallback(clean_content);
                }
                m_InputContent[0] = '\0';
                ImGui::SetKeyboardFocusHere(-1);
            }
        }

        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar();

        ImGui::End();
        ImGui::PopStyleVar();
    }

    void UIChatWindow::addMessage(const message::Message &message)
    {
        m_Messages.push_back(message);
    }

    void UIChatWindow::addUserProfilePictureTextureID(std::string_view username, ProfilePictureTextureID id)
    {
        m_ProfilePictureIds[username.data()] = id;
    }

    void UIChatWindow::setOnMessageSend(OnSendCallback &&callback)
    {
        m_OnSendCallback = std::move(callback);
    }

    void UIChatWindow::drawCircularProfile(std::string_view username, ImVec2 pos, float size, std::string_view placeholderText)
    {
        ImDrawList *drawList = ImGui::GetWindowDrawList();
        float rounding = size * 0.5f;
        ImVec2 center = ImVec2(pos.x + rounding, pos.y + rounding);

        if (m_ProfilePictureIds[std::string(username)] == 0)
        {
            drawList->AddCircleFilled(center, rounding, ImColor(60, 60, 60, 255), 64);

            ImVec2 textSize = ImGui::CalcTextSize(placeholderText.data());
            drawList->AddText(ImVec2(center.x - textSize.x * 0.5f, center.y - textSize.y * 0.5f),
                              ImColor(200, 200, 200, 255), placeholderText.data());
        } else
        {
            ImVec2 maxPos = ImVec2(pos.x + size, pos.y + size);
            drawList->AddImageRounded(
                m_ProfilePictureIds[username.data()],
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
