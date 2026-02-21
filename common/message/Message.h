#ifndef SLINK_MESSAGE_H
#define SLINK_MESSAGE_H 

#include <string>
#include <timestamp/Timestamp.h>
#include <protocol/Command.h>
#include <utility/benchmark/Benchmark.h>

namespace sLink::message
{
    class Message
    {
    public:
        Message() = default;

        Message(protocol::Command command, std::string_view senderName, std::string_view content);

        Message(protocol::Command command, std::string_view senderName, std::string_view content, utility::Timestamp timestamp);

        void setCommand(protocol::Command command);

        void setSenderName(std::string_view name);

        void setContent(std::string_view content);

        protocol::Command getCommand() const;

        std::string_view getSenderName() const;

        std::string_view getContent() const;

        utility::Timestamp getTimestamp() const;

        std::string serialize() const;

        static Message deserialize(std::string_view raw);

    private:
        protocol::Command m_Command;

        std::string m_SenderName;

        std::string m_Content;

        utility::Timestamp m_Timestamp;

        static constexpr std::string_view s_JSONCommandSelector = "command";

        static constexpr std::string_view s_JSONSenderNameSelector = "sender_name";

        static constexpr std::string_view s_JSONContentSelector = "content";

        static constexpr std::string_view s_JSONTimestampSelector = "time_stamp";

        static constexpr std::string_view s_BenchmarkOutputColor = SLINK_CL_CLR_CYAN;
    };
}

#endif