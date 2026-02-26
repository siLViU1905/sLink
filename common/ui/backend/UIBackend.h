#ifndef SLINK_UIBACKEND_H
#define SLINK_UIBACKEND_H

#include <imgui.h>

namespace sLink::ui
{
    class UIBackend
    {
    public:
        using UIRenderData = ImDrawData;

        UIBackend();

        static void begin_frame();

        static void end_frame();

        static UIRenderData *get_ui_render_data();

        void updateDisplay(int width, int height);

        ~UIBackend();

    private:
        /*
         *This theme was taken from the official imgui repo -> https://github.com/ocornut/imgui/issues/707#issuecomment-2732535348
         */
        static void apply_theme();

        ImGuiContext *m_Context;

        ImGuiIO *m_Io;
    };
}

#endif