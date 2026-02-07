#include "UIChatWindow.h"

#include <imgui.h>

namespace sLink::ui::component
{
	UIChatWindow::UIChatWindow():m_InputContent(255, 0)
	{
	}

	void UIChatWindow::render()
	{
		ImGui::Begin("Chat Window");

		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false);

		for (const auto& msg : m_Messages) 
		{
			ImGui::Text("[%lld] %s: %s", msg.getTimestamp(), msg.getSenderName().data(), msg.getContent().data());
		}

		ImGui::EndChild();

		if (ImGui::InputText("##input", m_InputContent.data(), 256, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			//TODO: add callback to send message
			m_InputContent[0] = '\0';
		}

		ImGui::End();
	}

	void UIChatWindow::addMessage(const message::Message& message)
	{
		m_Messages.push_back(message);
	}
}
