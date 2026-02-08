#ifndef SLINK_CLIENT_H
#define SLINK_CLIENT_H

#include <asio.hpp>
#include <utility/safe_queue/SafeQueue.h>

#include "message/Message.h"

namespace sLink::client
{
	class Client
	{
	public:
		Client(asio::io_context& ctx);

		void setUsername(std::string_view name);

		std::string_view getUsername() const;

		void connect(std::string_view host, std::string_view port);

		void send(const message::Message& message);

		bool isConnected() const;

		utility::SafeQueue<std::string>& getInbox();

	private:
		void onConnect(asio::ip::tcp::resolver::results_type endpoints);

		void onWrite();

		void onRead();

		void onJoin();

		std::string m_Username;

		asio::io_context& m_IOContext;

		asio::executor_work_guard<asio::io_context::executor_type> m_WorkGuard;

		asio::ip::tcp::socket m_Socket;

		utility::SafeQueue<std::string> m_Inbox;

		utility::SafeQueue<std::string> m_Outbox;

		asio::streambuf m_ReadBuffer;

		bool m_IsWriting;

		std::string m_CurrentMessage;
	};
}

#endif