#include "Client.h"

namespace sLink::client
{
	Client::Client(asio::io_context& ctx) :m_IOContext(ctx), m_Socket(ctx), m_IsWriting(false)
	{
	}

	void Client::setUsername(std::string_view name)
	{
		m_Username = name;
	}

	std::string_view Client::getUsername() const
	{
		return m_Username;
	}

	void Client::connect(std::string_view host, std::string_view port)
	{
		asio::ip::tcp::resolver resolver(m_IOContext);

		auto endpoints = resolver.resolve(host, port);

		onConnect(endpoints);
	}

	void Client::send(const message::Message& message)
	{
		m_Outbox.push(message.serialize());

		asio::post(m_IOContext, [this]()
			{
				if (!m_IsWriting)
					onWrite();
			});
	}

	bool Client::isConnected() const
	{
		return m_Socket.is_open();
	}

	void Client::onConnect(asio::ip::tcp::resolver::results_type endpoints)
	{
		asio::async_connect(m_Socket, endpoints,
			[this](std::error_code ec, asio::ip::tcp::endpoint)
			{
				if (!ec)
				{
					onRead();
				}
			});
	}

	void Client::onWrite()
	{
		auto msg = m_Outbox.tryPop();

		if (!msg)
		{
			m_IsWriting = false;
			return;
		}

		m_IsWriting = true;

		m_CurrentMessage = *msg + "\n";

		asio::async_write(m_Socket, asio::buffer(m_CurrentMessage),
			[this](std::error_code ec, size_t)
			{
				if (!ec)
					onWrite();
				else
				{
					m_IsWriting = false;
					m_Socket.close();
				}
			});
	}

	void Client::onRead()
	{
		asio::async_read_until(m_Socket, m_ReadBuffer, '\n',
			[this](std::error_code ec, size_t length)
			{
				if (!ec)
				{
					std::istream is(&m_ReadBuffer);

					std::string line;

					std::getline(is, line);

					m_Inbox.push(std::move(line));

					onRead();
				}
				else
					m_Socket.close();
			});
	}
}