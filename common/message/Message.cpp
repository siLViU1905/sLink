#include "Message.h"

#include <chrono>
#include <nlohmann/json.hpp>
#include <utility/benchmark/Benchmark.h>

namespace sLink::message
{
    Message::Message(protocol::Command command, std::string_view senderName, std::string_view content)
        : m_Command(command), m_SenderName(senderName), m_Content(content)
    {
    }

    Message::Message(protocol::Command command, std::string_view senderName, std::string_view content, utility::Timestamp timestamp)
        : m_Command(command), m_SenderName(senderName), m_Content(content), m_Timestamp(timestamp)
    {
    }

    void Message::setCommand(protocol::Command command)
    {
        m_Command = command;
    }

    void Message::setSenderName(std::string_view name)
    {
        m_SenderName = name;
    }

    void Message::setContent(std::string_view content)
    {
        m_Content = content;
    }

    protocol::Command Message::getCommand() const
    {
        return m_Command;
    }

    std::string_view Message::getSenderName() const
    {
        return m_SenderName;
    }

    std::string_view Message::getContent() const
    {
        return m_Content;
    }

    utility::Timestamp Message::getTimestamp() const
    {
        return m_Timestamp;
    }

    std::string Message::serialize() const
    {
        SLINK_START_BENCHMARK;

        nlohmann::json js;

        js[s_JSONCommandSelector] = m_Command;

        js[s_JSONSenderNameSelector] = m_SenderName;

        js[s_JSONContentSelector] = m_Content;

        js[s_JSONTimestampSelector] = m_Timestamp.getMs();

        SLINK_END_BENCHMARK("[Message]", "serialize")

        return js.dump();
    }

    Message Message::deserialize(std::string_view raw)
    {
        SLINK_START_BENCHMARK

        auto js = nlohmann::json::parse(raw);

        auto command = js[s_JSONCommandSelector].get<protocol::Command>();

        auto senderName = js[s_JSONSenderNameSelector].get<std::string>();

        auto content = js[s_JSONContentSelector].get<std::string>();

        auto timestamp = js[s_JSONTimestampSelector].get<int64_t>();

        SLINK_END_BENCHMARK("[Message]", "deserialize")

        return {command, senderName, content, timestamp};
    }
}
