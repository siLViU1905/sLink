#ifndef SLINK_SESSION_H
#define SLINK_SESSION_H

#include <asio.hpp>

#include "safe_queue/SafeQueue.h"

namespace sLink::session
{
	class Session : public std::enable_shared_from_this<Session>
	{
	public:
		Session(asio::ip::tcp::socket&& socket, utility::SafeQueue<std::string>& inbox);

		void start();

		void send(const std::string& message);
	private:
		void onRead();

		void onWrite();

		asio::ip::tcp::socket m_Socket;

		asio::streambuf m_Buffer;

		utility::SafeQueue<std::string>& m_Inbox;

		std::queue<std::string> m_WriteQueue;
	};
}

#endif