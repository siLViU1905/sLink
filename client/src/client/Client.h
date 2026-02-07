#ifndef SLINK_CLIENT_H
#define SLINK_CLIENT_H

#include <asio.hpp>
#include <utility/safe_queue/SafeQueue.h>

namespace sLink::client
{
	class Client
	{
	public:
		Client(asio::io_context& ctx);

		void connect(std::string_view host, std::string_view port);

		void send(std::string msg);

		bool isConnected() const;
	private:
		void onConnect(asio::ip::tcp::resolver::results_type endpoints);

		void onWrite();

		void onRead();

		asio::io_context& m_IOContext;

		asio::ip::tcp::socket m_Socket;

		utility::SafeQueue<std::string> m_Inbox;

		utility::SafeQueue<std::string> m_Outbox;

		asio::streambuf m_ReadBuffer;

		bool m_IsWriting;
	};
}

#endif