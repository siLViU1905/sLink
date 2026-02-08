#include "UIChatWindow.h"

#include <imgui.h>

namespace sLink::ui::component
{
    UIChatWindow::UIChatWindow() : m_InputContent(255, 0)
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

        float scaled_line_height = ImGui::GetTextLineHeightWithSpacing() * s_FontScale;
        float footer_height = s_ButtonHeight + ImGui::GetStyle().ItemSpacing.y * 2;

        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height), false);

        for (const auto& message : m_Messages)
        {
            ImGui::BeginGroup();

            ImGui::TextColored(s_ColorTimestamp, "[%s]", message.getTimestamp().toString().c_str());

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

        bool input_submitted = ImGui::InputText("##input", m_InputContent.data(), 256, ImGuiInputTextFlags_EnterReturnsTrue);

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

    void UIChatWindow::setOnMessageSend(OnSendCallback &&callback)
    {
        m_OnSendCallback = std::move(callback);
    }
}
