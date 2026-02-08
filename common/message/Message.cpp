#include "Message.h"

#include <chrono>
#include <nlohmann/json.hpp>

namespace sLink::message
{
    Message::Message(std::string_view senderName, std::string_view content)
        : m_SenderName(senderName), m_Content(content)
    {
    }

    Message::Message(std::string_view senderName, std::string_view content, utility::Timestamp timestamp)
        : m_SenderName(senderName), m_Content(content), m_Timestamp(timestamp)
    {
    }

    void Message::setSenderName(std::string_view name)
    {
        m_SenderName = name;
    }

    void Message::setContent(std::string_view content)
    {
        m_Content = content;
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
        nlohmann::json js;

        js[s_JSONSenderNameSelector] = m_SenderName;

        js[s_JSONContentSelector] = m_Content;

        js[s_JSONTimestampSelector] = m_Timestamp.getMs();

        return js.dump();
    }

    Message Message::deserialize(std::string_view raw)
    {
        auto js = nlohmann::json::parse(raw);

        auto senderName = js[s_JSONSenderNameSelector].get<std::string>();

        auto content = js[s_JSONContentSelector].get<std::string>();

        auto timestamp = js[s_JSONTimestampSelector].get<int64_t>();

        return {senderName, content, timestamp};
    }

    int64_t Message::getTimeSinceEpochMS()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }
}
