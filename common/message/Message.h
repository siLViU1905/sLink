#ifndef SLINK_MESSAGE_H 
#define SLINK_MESSAGE_H 

#include <string>

namespace sLink::message
{
	class Message
	{
	public:
		Message();

		Message(std::string_view senderName, std::string_view content);

		Message(std::string_view senderName, std::string_view content, int64_t timestamp);

		void setSenderName(std::string_view name);

		void setContent(std::string_view content);

		std::string_view getSenderName() const;

		std::string_view getContent() const;

		int64_t getTimestamp() const;

		std::string serialize() const;

		static Message deserialize(std::string_view raw);

	private:
		static int64_t getTimeSinceEpochMS();

		std::string m_SenderName;

		std::string m_Content;

		int64_t m_Timestamp;

		static constexpr std::string_view s_JSONSenderNameSelector = "sender_name";

		static constexpr std::string_view s_JSONContentSelector = "content";

		static constexpr std::string_view s_JSONTimestampSelector = "time_stamp";
	};
}

#endif