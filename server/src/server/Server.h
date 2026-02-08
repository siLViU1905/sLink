#ifndef SLINK_SERVER_H
#define SLINK_SERVER_H

#include <asio.hpp>
#include <utility/safe_queue/SafeQueue.h>

#include "../session/Session.h"

namespace sLink::server
{
	class Server
	{
	public:
		Server(asio::io_context& ctx, uint16_t port);

		//void broadcast(std::string msg);

	private:
		void onAccept();

		asio::io_context& m_IOContext;

		asio::ip::tcp::acceptor m_Acceptor;

		utility::SafeQueue<std::string> m_Inbox;

		utility::SafeQueue<std::string> m_Outbox;

		asio::streambuf m_ReadBuffer;

		std::vector<std::shared_ptr<session::Session>> m_Sessions;

		bool m_IsWriting;
	};
}

#endif