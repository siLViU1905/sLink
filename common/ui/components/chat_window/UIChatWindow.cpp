#include "UIChatWindow.h"

#include <imgui.h>

namespace sLink::ui::component
{
	UIChatWindow::UIChatWindow():m_InputContent(255, 0)
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

        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false);
        for (const auto& msg : m_Messages)
        {
            ImGui::Text("[%lld] %s: %s", msg.getTimestamp(), msg.getSenderName().data(), msg.getContent().data());
        }
        ImGui::EndChild();

        ImGui::Separator();

        constexpr float button_width = 60.f;

        float spacing = ImGui::GetStyle().ItemSpacing.x;

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - button_width - spacing);

        bool input_submitted = ImGui::InputText("##input", m_InputContent.data(), 256, ImGuiInputTextFlags_EnterReturnsTrue);

        ImGui::PopItemWidth();

        ImGui::SameLine();

        if (ImGui::Button("Send", ImVec2(button_width, 0)) || input_submitted)
            if (m_InputContent[0] != '\0')
                m_InputContent[0] = '\0';

        ImGui::End();
        ImGui::PopStyleVar(); 
    }

	void UIChatWindow::addMessage(const message::Message& message)
	{
		m_Messages.push_back(message);
	}
}
