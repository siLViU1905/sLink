#ifndef SLINK_UICLIENTLOGGING_H
#define SLINK_UICLIENTLOGGING_H
#include <string>
#include <vector>

#include "components/UIComponent.h"
#include <imgui.h>

namespace sLink::ui::component
{
    class UIClientLogging : UIComponent
    {
    public:
        void render() override;

        void logClientConnected(std::string_view username);

        void logClientDisconnected(std::string_view username);

    private:
        static constexpr float s_FontScale = 1.15f;

        static constexpr float s_WindowWidth = 200.f;

        static constexpr ImVec4 s_ColorConnected = { 0.2f, 0.9f, 0.2f, 1.f };

        static constexpr ImVec4 s_ColorDisconnected = { 0.9f, 0.2f, 0.2f, 1.f };

        static constexpr ImVec4 s_ColorHeader = { 0.2f, 0.7f, 0.9f, 1.f };

        struct ClientLog
        {
            enum class State
            {
                CONNECTED,
                DISCONNECTED
            };

            std::string m_Username;

            State m_State;
        };

        std::vector<ClientLog> m_Logs;
    };
}

#endif
