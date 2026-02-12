#include "UIPortSelect.h"

#include <imgui.h>

namespace sLink::server::ui::component
{
    UIPortSelect::UIPortSelect():m_ShowErrorPopup(false)
    {
        m_InputPort.reserve(5);
    }

    void UIPortSelect::render()
    {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(s_WindowWidth, s_WindowHeight));

        ImGui::Begin("Select the host port", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, s_InputPaddingY));

        ImGui::Text("Port:");
        ImGui::InputText("##port", m_InputPort.data(), 6, ImGuiInputTextFlags_CharsDecimal);

        ImGui::PopStyleVar();
        ImGui::Dummy(ImVec2(0, s_ItemSpacing));

        if (ImGui::Button("Start", ImVec2(-1, 40)))
        {
            std::string portStr(m_InputPort.c_str());

            if (portStr.empty())
            {
                m_ErrorMessage = "Port field is empty!";
                m_ShowErrorPopup = true;
            }
            else if (m_OnPortInputCallback)
            {
                m_OnPortInputCallback(portStr);
            }
        }

        if (m_ShowErrorPopup)
        {
            ImGui::OpenPopup("Port Error");
        }

        if (ImGui::BeginPopupModal("Port Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
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

    void UIPortSelect::setOnPortInput(OnPortInputCallback &&callback)
    {
        m_OnPortInputCallback = std::move(callback);
    }
}
