#include "Server.h"


namespace sLink::server
{
    Server::Server(asio::io_context &ctx, db::Database &database) : m_IOContext(ctx),
                                                                    m_Database(database),
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

    void Server::broadcast(const message::Message &message)
    {
        SLINK_START_BENCHMARK

        std::scoped_lock lock(m_SessionsMutex);
        for (auto &session: m_Sessions)
            session->send(message);

        SLINK_END_BENCHMARK("[Server]", "broadcast", s_BenchmarkOutputColor)
    }

    void Server::update()
    {
        while (auto response = m_Database.getUserResponses().tryPop())
        {
            std::shared_ptr<session::Session> session; {
                std::scoped_lock lock(m_PendingSessionsMutex);
                auto it = m_PendingSessions.find(response->m_Username);
                if (it != m_PendingSessions.end())
                {
                    session = it->second;
                    m_PendingSessions.erase(it);
                }
            }

            if (session)
                switch (response->m_Type)
                {
                    case db::Database::Response::ResponseType::LOGIN_SUCCESS:
                        if (!isUserConnected(session->getUser()))
                            onClientAccept(session);
                        else
                            onClientReject(session, "User is already connected");

                        break;

                    case db::Database::Response::ResponseType::LOGIN_FAIL:
                        onClientReject(session, response->m_Message);

                        break;
                    case db::Database::Response::ResponseType::REGISTER_SUCCESS:
                        if (!isUserConnected(session->getUser()))
                            onClientAccept(session);
                        else
                            onClientReject(session, "User is already connected");

                        break;

                    case db::Database::Response::ResponseType::REGISTER_FAIL:
                        onClientReject(session, response->m_Message);

                        break;
                }
        }

        while (auto message = m_Inbox.tryPop())
        {
            broadcast(message::Message::deserialize(*message));

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

    utility::SafeQueue<user::User> &Server::getDbUsernameInbox()
    {
        return m_DbUserInbox;
    }

    utility::SafeQueue<std::string> &Server::getDbMessageInbox()
    {
        return m_DbMessageInbox;
    }

    void Server::onAccept()
    {
        SLINK_START_BENCHMARK

        m_Acceptor->async_accept([this](std::error_code ec, asio::ip::tcp::socket socket)
        {
            if (!ec)
            {
                auto session = std::make_shared<session::Session>(std::move(socket), m_Inbox);

                session->setOnLoginInfoSentCallback([this, session](const user::User &user)
                {
                    m_PendingSessions[user.getUsername().data()] = session;

                    m_Database.requestUserLogin(user);
                });

                session->setOnRegisterInfoSentCallback([this, session](const user::User &user)
                {
                    m_PendingSessions[user.getUsername().data()] = session;

                    m_Database.requestUserRegister(user);
                });

                session->setOnDisconnectCallback([this, session]()
                {
                    onClientDisconnected(session);
                });

                session->start();
            }

            onAccept();
        });

        SLINK_END_BENCHMARK("[Server]", "onAccept", s_BenchmarkOutputColor)
    }

    void Server::onClientAccept(const std::shared_ptr<session::Session> &session)
    { {
            std::scoped_lock lock(m_SessionsMutex);

            m_Sessions.push_back(session);
        }
        m_PendingUsernames.push(session->getUser().getUsername().data());

        session->send({protocol::Command::LOGIN_RESPONSE_ACCEPT, "", "Successfully connected to the server"});
    }

    void Server::onClientDisconnected(const std::shared_ptr<session::Session> &session)
    {
        m_DisconnectedUsernames.push(std::string(session->getUser().getUsername()));

        std::erase(m_Sessions, session);
    }

    void Server::onClientReject(const std::shared_ptr<session::Session> &session, std::string_view reason)
    {
        message::Message message(protocol::Command::LOGIN_RESPONSE_REJECT, "", reason);

        session->send(message);

        session->disconnectAfterWrite();

        m_DisconnectedUsernames.push(std::format("Rejected: {} reason: {}", session->getUser().getUsername(), reason));
    }

    bool Server::isUserConnected(const user::User &user)
    {
        std::scoped_lock lock(m_SessionsMutex);

        SLINK_START_BENCHMARK

        auto it = std::ranges::find_if(m_Sessions, [&user](const auto &session)
        {
            return session->getUser().getUsername() == user.getUsername();
        });

        SLINK_END_BENCHMARK("[Server]", "isUserConnected", s_BenchmarkOutputColor)

        return it != m_Sessions.end();
    }

    void Server::kickUser(const user::User &user, std::string_view reason)
    {
        std::scoped_lock lock(m_SessionsMutex);

        SLINK_START_BENCHMARK

        auto it = std::ranges::find_if(m_Sessions, [&user](const auto &session)
        {
            return session->getUser().getUsername() == user.getUsername();
        });

        auto& session = *it->get();

        message::Message message(protocol::Command::SERVER_KICK_REQUEST, "", reason);

        session.send(message);

        session.disconnectAfterWrite();

        m_Sessions.erase(it);

        SLINK_END_BENCHMARK("[Server]", "kickUser", s_BenchmarkOutputColor)

        m_DisconnectedUsernames.push(user.getUsername().data());
    }
}
