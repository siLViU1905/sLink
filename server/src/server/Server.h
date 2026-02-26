#ifndef SLINK_SERVER_H
#define SLINK_SERVER_H

#include <asio.hpp>
#include <utility/safe_queue/SafeQueue.h>
#include "../session/Session.h"
#include <expected>

#include "../database/Database.h"
#include <utility/benchmark/Benchmark.h>

namespace sLink::server
{
    class Server
    {
    private:
        static constexpr std::string_view s_BenchmarkOutputColor = SLINK_CL_CLR_MAGENTA;

    public:
        Server(asio::io_context &ctx, db::Database& database);

        std::expected<std::string, std::string> startHost(uint16_t port);

        void broadcast(const message::Message &message);

        void update();

        utility::SafeQueue<std::string> &getPendingUsernames();

        utility::SafeQueue<std::string> &getDisconnectedUsernames();

        utility::SafeQueue<user::User> &getDbUsernameInbox();

        utility::SafeQueue<std::string> &getDbMessageInbox();

    private:
        void onAccept();

        void onClientAccept(const std::shared_ptr<session::Session> &session);

        void onClientDisconnected(const std::shared_ptr<session::Session> &session);

        void onClientReject(const std::shared_ptr<session::Session> &session, std::string_view reason);

        bool isUserConnected(const user::User& user);

        asio::io_context &m_IOContext;

        db::Database& m_Database;

        asio::executor_work_guard<asio::io_context::executor_type> m_WorkGuard;

        std::unique_ptr<asio::ip::tcp::acceptor> m_Acceptor;

        utility::SafeQueue<std::string> m_Inbox;

        utility::SafeQueue<std::string> m_Outbox;

        asio::streambuf m_ReadBuffer;

        std::vector<std::shared_ptr<session::Session> > m_Sessions;

        std::mutex m_SessionsMutex;

        std::unordered_map<std::string, std::shared_ptr<session::Session>> m_PendingSessions;

        std::mutex m_PendingSessionsMutex;

        utility::SafeQueue<std::string> m_PendingUsernames;

        utility::SafeQueue<std::string> m_DisconnectedUsernames;

        utility::SafeQueue<user::User> m_DbUserInbox;

        utility::SafeQueue<std::string> m_DbMessageInbox;

        bool m_IsWriting;
    };
}

#endif