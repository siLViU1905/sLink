#include "Server.h"

namespace sLink::server
{
    Server::Server(asio::io_context &ctx) : m_IOContext(ctx),
                                            m_WorkGuard(asio::make_work_guard(ctx)),
                                            m_IsWriting(false)
    {
    }

    std::expected<std::string, std::string> Server::startHost(uint16_t port)
    {
        try
        {
            m_Acceptor = std::make_unique<asio::ip::tcp::acceptor>(
                m_IOContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)
            );

            onAccept();

            return {std::format("Server started on port {}", port)};
        } catch (const asio::system_error &e)
        {
            return std::unexpected(std::format("Failed to start server on port {}", port));
        }
    }

    void Server::broadcast(const std::string &message)
    {
        for (auto &session: m_Sessions)
            session->send(message);
    }

    void Server::update()
    {
        while (auto message = m_Inbox.tryPop())
        {
            broadcast(*message);

            m_DbMessageInbox.push(*message);
        }
    }

    utility::SafeQueue<std::string> &Server::getPendingUsernames()
    {
        return m_PendingUsernames;
    }

    utility::SafeQueue<std::string> &Server::getDisconnectedUsernames()
    {
        return m_DisconnectedUsernames;
    }

    utility::SafeQueue<std::string> & Server::getDbUsernameInbox()
    {
        return m_DbUsernameInbox;
    }

    utility::SafeQueue<std::string> & Server::getDbMessageInbox()
    {
        return m_DbMessageInbox;
    }

    void Server::onAccept()
    {
        m_Acceptor->async_accept([this](std::error_code ec, asio::ip::tcp::socket socket)
        {
            if (!ec)
            {
                m_Sessions.push_back(std::make_shared<session::Session>(std::move(socket), m_Inbox));

                auto &session = m_Sessions.back();

                session->setOnUsernameSentCallback([this](std::string_view username)
                {
                    m_PendingUsernames.push(std::string(username));

                    m_DbUsernameInbox.push(std::string(username));
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

    void Server::onClientDisconnected(const std::shared_ptr<session::Session> &session)
    {
        m_DisconnectedUsernames.push(std::string(session->getUsername()));

        std::erase(m_Sessions, session);
    }
}