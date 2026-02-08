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
	private:
		void onRead();

		asio::ip::tcp::socket m_Socket;

		asio::streambuf m_Buffer;

		utility::SafeQueue<std::string>& m_Inbox;
	};
}

#endif