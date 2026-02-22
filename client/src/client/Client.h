#ifndef SLINK_CLIENT_H
#define SLINK_CLIENT_H

#include <asio.hpp>
#include <expected>
#include <utility/safe_queue/SafeQueue.h>

#include "message/Message.h"
#include <utility/benchmark/Benchmark.h>

namespace sLink::client
{
    class Client
    {
    private:
        static constexpr std::string_view s_BenchmarkOutputColor = SLINK_CL_CLR_GREEN;

    public:
        Client(asio::io_context &ctx);

        void setUsername(std::string_view name);

        void setPassword(std::string_view password);

        std::string_view getUsername() const;

        std::expected<std::string, std::string> connect(std::string_view host, std::string_view port, protocol::Command joinType);

        void send(const message::Message &message);

        bool isConnected() const;

        utility::SafeQueue<std::string> &getInbox();

    private:
        std::expected<std::string, std::string> onConnect(asio::ip::tcp::resolver::results_type endpoints, protocol::Command joinType);

        void onWrite();

        void onRead();

        void onJoin(protocol::Command joinType);

        std::string m_Username;

        std::string m_Password;

        asio::io_context &m_IOContext;

        asio::executor_work_guard<asio::io_context::executor_type> m_WorkGuard;

        asio::ip::tcp::socket m_Socket;

        utility::SafeQueue<std::string> m_Inbox;

        utility::SafeQueue<std::string> m_Outbox;

        asio::streambuf m_ReadBuffer;

        bool m_IsWriting;

        bool m_ConnectionFailed;

        std::string m_CurrentMessage;
    };
}

#endif