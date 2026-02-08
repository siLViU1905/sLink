#include "Session.h"

namespace sLink::session
{
	Session::Session(asio::ip::tcp::socket&& socket, utility::SafeQueue<std::string>& inbox)
		:m_Socket(std::move(socket)), m_Inbox(inbox)
	{
	}

	void Session::start()
	{
		onRead();
	}

	void Session::send(const std::string& message)
	{
		auto self(shared_from_this());

		asio::post(m_Socket.get_executor(), [this, self, message]()
			{
				bool idle = m_WriteQueue.empty();

				m_WriteQueue.push(message + "\n");

				if (idle)
					onWrite();
			});
	}

	void Session::onRead()
	{
		auto self(shared_from_this());

		asio::async_read_until(m_Socket, m_Buffer, '\n',
			[this, self](std::error_code ec, size_t)
			{
				if (!ec)
				{
					std::istream is(&m_Buffer);

					std::string msg;

					std::getline(is, msg);

					m_Inbox.push(msg);

					onRead();
				}
			});
	}

	void Session::onWrite()
	{
		auto self(shared_from_this());

		asio::async_write(m_Socket, asio::buffer(m_WriteQueue.front()),
			[this, self](std::error_code ec, size_t)
			{
				if (!ec)
				{
					m_WriteQueue.pop();

					if (!m_WriteQueue.empty())
						onWrite();
				}
			});
	}
}
