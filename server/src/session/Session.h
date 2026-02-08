#ifndef SLINK_SESSION_H
#define SLINK_SESSION_H

#include <asio.hpp>

namespace sLink::session
{
	class Session : public std::enable_shared_from_this<Session>
	{
	public:
		Session(asio::ip::tcp::socket&& socket);

		void start();
	private:
		void onRead();

		asio::ip::tcp::socket m_Socket;

		asio::streambuf m_Buffer;
	};
}

#endif