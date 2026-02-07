#include "Session.h"

namespace sLink::session
{
	Session::Session(asio::ip::tcp::socket&& socket) :m_Socket(std::move(socket))
	{
	}

	void Session::start()
	{
		onRead();
	}

	void Session::onRead()
	{
		asio::async_read_until(m_Socket, m_Buffer, '\n',
			[this](std::error_code ec, size_t)
			{
				std::istream is(&m_Buffer);

				std::string msg;

				std::getline(is, msg);

				onRead();
			});
	}
}
