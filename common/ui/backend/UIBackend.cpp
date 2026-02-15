#include "UIBackend.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "imgui_internal.h"

namespace sLink::ui
{
    UIBackend::UIBackend()
    {
        IMGUI_CHECKVERSION();

        m_Context = ImGui::CreateContext();

        m_Io = &ImGui::GetIO();
    }

    void UIBackend::begin_frame()
    {
        ImGui_ImplVulkan_NewFrame();

        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
    }

    void UIBackend::end_frame()
    {
        ImGui::Render();
    }

    UIBackend::UIRenderData *UIBackend::get_ui_render_data()
    {
        return ImGui::GetDrawData();
    }

    void UIBackend::updateDisplay(int width, int height)
    {
        m_Io->DisplaySize = {
            static_cast<float>(width),
            static_cast<float>(height)
        };
    }

    UIBackend::~UIBackend()
    {
        ImGui::DestroyContext(m_Context);

        m_Context = nullptr;

        m_Io = nullptr;
    }
}