#ifndef SLINK_SESSION_H
#define SLINK_SESSION_H

#include <asio.hpp>

#include "message/Message.h"
#include "safe_queue/SafeQueue.h"
#include <utility/benchmark/Benchmark.h>

namespace sLink::session
{
    class Session : public std::enable_shared_from_this<Session>
    {
    private:
        static constexpr std::string_view s_BenchmarkOutputColor = SLINK_CL_CLR_RED;

    public:
        using OnUsernameSentCallback = std::move_only_function<void(std::string_view)>;

        using OnDisconnectCallback = std::move_only_function<void(std::string_view)>;

        Session(asio::ip::tcp::socket &&socket, utility::SafeQueue<std::string> &inbox);

        void start();

        void disconnect();

        void disconnectAfterWrite();

        void send(const message::Message &message);

        void setUsername(std::string_view username);

        std::string_view getUsername() const;

        void setOnUsernameSentCallback(OnUsernameSentCallback &&callback);

        void setOnDisconnectCallback(OnDisconnectCallback &&callback);

    private:
        void onRead();

        void onWrite();

        void handleMessage();

        std::string m_Username;

        asio::ip::tcp::socket m_Socket;

        asio::streambuf m_Buffer;

        utility::SafeQueue<std::string> &m_Inbox;

        std::queue<std::string> m_WriteQueue;

        OnUsernameSentCallback m_OnUsernameSentCallback;

        OnDisconnectCallback m_OnDisconnectCallback;

        bool m_ShouldDisconnectAfterWrite;
    };
}

#endif
