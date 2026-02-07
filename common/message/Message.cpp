#include "Message.h"

#include <chrono>

namespace sLink::message
{
	Message::Message()
	{
		m_Timestamp = getTimeSinceEpochMS();
	}

	Message::Message(std::string_view senderName, std::string_view content):
		m_SenderName(senderName), m_Content(content)
	{
		m_Timestamp = getTimeSinceEpochMS();
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

	int64_t Message::getTimestamp() const
	{
		return m_Timestamp;
	}

	int64_t Message::getTimeSinceEpochMS()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()
		).count();
	}
}
