#include "Server.h"
#include <nlohmann/json.hpp>
#include <utility/base64/Base64.h>

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
            session->send(message, session);

        SLINK_END_BENCHMARK("[Server]", "broadcast", s_BenchmarkOutputColor)
    }

    void Server::update()
    {
        while (auto response = m_Database.getUserResponses().tryPop())
        {
            if (response->m_Type == db::Database::Response::ResponseType::USER_PROFILE_PICTURE)
            {
                handleProfilePictureDataRequest(*response);
                continue;
            }

            handlePendingSessions(*response);
        }

        while (auto rawMessage = m_Inbox.tryPop())
        {
            auto message = message::Message::deserialize(*rawMessage);

            broadcast(message);

            m_Database.requestMessageSave(message);
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

                session->setOnProfilePictureSentCallback(
                    [this, session](const user::User &user, std::string_view content)
                    {
                        m_Database.requestProfilePictureSave(user, content);
                    });

                session->setOnDisconnectCallback([this, session]()
                {
                    onClientDisconnected(session);
                });

                session->start(session);
            }

            onAccept();
        });

        SLINK_END_BENCHMARK("[Server]", "onAccept", s_BenchmarkOutputColor)
    }

    void Server::onClientAccept(const std::shared_ptr<session::Session> &session)
    { {
            std::scoped_lock lock(m_SessionsMutex);

            for (const auto &s: m_Sessions)
            {
                m_Database.requestUserProfilePicture(s->getUser(), session->getUser().getUsername());

                m_Database.requestUserProfilePicture(session->getUser(), s->getUser().getUsername());
            }

            m_Sessions.push_back(session);
        }
        m_PendingUsernames.push(session->getUser().getUsername().data());

        session->send({protocol::Command::LOGIN_RESPONSE_ACCEPT, "", "Successfully connected to the server"}, session);
    }

    void Server::onClientDisconnected(const std::shared_ptr<session::Session> &session)
    {
        m_DisconnectedUsernames.push(std::string(session->getUser().getUsername()));

        std::erase(m_Sessions, session);
    }

    void Server::onClientReject(const std::shared_ptr<session::Session> &session, std::string_view reason)
    {
        message::Message message(protocol::Command::LOGIN_RESPONSE_REJECT, "", reason);

        session->send(message, session);

        session->disconnectAfterWrite(session);

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

    void Server::handleProfilePictureDataRequest(const db::Database::Response &response)
    {
        std::scoped_lock lock(m_SessionsMutex);
        auto it = std::ranges::find_if(m_Sessions, [&](const auto &s)
        {
            return s->getUser().getUsername() == response.m_Recipient;
        });

        if (it != m_Sessions.end())
        {
            auto &session = *it;
            std::span pixelData(
                reinterpret_cast<const uint8_t *>(response.m_Message.data()),
                response.m_Message.size()
            );
            std::string encodedPixels = utility::base64::encode(pixelData);

            nlohmann::json js;
            js["base64_pixels"] = encodedPixels;

            session->send({protocol::Command::PROFILE_PICTURE, response.m_Username, js.dump()},
                          session);
        }
    }

    void Server::handlePendingSessions(const db::Database::Response &response)
    {
        std::shared_ptr<session::Session> session; {
            std::scoped_lock lock(m_PendingSessionsMutex);
            auto it = m_PendingSessions.find(response.m_Username);
            if (it != m_PendingSessions.end())
            {
                session = it->second;
                m_PendingSessions.erase(it);
            }
        }

        switch (response.m_Type)
        {
            case db::Database::Response::ResponseType::LOGIN_SUCCESS:
                if (!isUserConnected(session->getUser()))
                    onClientAccept(session);
                else
                    onClientReject(session, "User is already connected");

                break;

            case db::Database::Response::ResponseType::LOGIN_FAIL:
                onClientReject(session, response.m_Message);

                break;
            case db::Database::Response::ResponseType::REGISTER_SUCCESS:
                if (!isUserConnected(session->getUser()))
                    onClientAccept(session);
                else
                    onClientReject(session, "User is already connected");

                break;

            case db::Database::Response::ResponseType::REGISTER_FAIL:
                onClientReject(session, response.m_Message);

                break;
            default:
                break;
        }
    }

    void Server::kickUser(const user::User &user, std::string_view reason)
    {
        std::scoped_lock lock(m_SessionsMutex);

        SLINK_START_BENCHMARK

        auto it = std::ranges::find_if(m_Sessions, [&user](const auto &session)
        {
            return session->getUser().getUsername() == user.getUsername();
        });

        auto &session = *it;

        message::Message message(protocol::Command::SERVER_KICK_REQUEST, "", reason);

        session->send(message, session);

        session->disconnectAfterWrite(session);

        m_Sessions.erase(it);

        SLINK_END_BENCHMARK("[Server]", "kickUser", s_BenchmarkOutputColor)

        m_DisconnectedUsernames.push(user.getUsername().data());
    }
}
