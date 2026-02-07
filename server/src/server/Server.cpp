#include "Server.h"

namespace sLink::server
{
	Server::Server(asio::io_context& ctx, uint16_t port) :m_IOContext(ctx),
		m_Acceptor(ctx, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
		m_IsWriting(false)
	{
		onAccept();
	}

	void Server::onAccept()
	{
		m_Acceptor.async_accept([this](std::error_code ec, asio::ip::tcp::socket socket) {
			if (!ec)
			{
				auto session = std::make_unique<session::Session>(std::move(socket));

				session->start();

				m_Sessions.push_back(std::move(session));
			}

			onAccept();
			});
	}
}
