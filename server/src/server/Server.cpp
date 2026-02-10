#include "Server.h"

namespace sLink::server
{
	Server::Server(asio::io_context& ctx, uint16_t port) :m_IOContext(ctx),
		m_Acceptor(ctx, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
		m_IsWriting(false)
	{
		onAccept();
	}

	void Server::broadcast(const std::string& message)
	{
		for (auto& session : m_Sessions)
			session->send(message);
	}

	void Server::update()
	{
		while (auto message = m_Inbox.tryPop())
			broadcast(*message);
	}

	utility::SafeQueue<std::string> & Server::getPendingUsernames()
	{
		return m_PendingUsernames;
	}

	utility::SafeQueue<std::string> & Server::getDisconnectedUsernames()
	{
		return m_DisconnectedUsernames;
	}

	void Server::onAccept()
	{
		m_Acceptor.async_accept([this](std::error_code ec, asio::ip::tcp::socket socket) {
			if (!ec)
			{
				m_Sessions.push_back(std::make_shared<session::Session>(std::move(socket), m_Inbox));

				auto& session = m_Sessions.back();

				session->setOnUsernameSentCallback([this](std::string_view username)
				{
					m_PendingUsernames.push(std::string(username));
				});

				session->setOnDisconnectCallback([this, session](std::string_view username)
				{
					onClientDisconnected(session);
				});

				session->start();
			}

			onAccept();
			});
	}

	void Server::onClientDisconnected(const std::shared_ptr<session::Session>& session)
	{
		m_DisconnectedUsernames.push(std::string(session->getUsername()));

		std::erase(m_Sessions, session);
	}
}
