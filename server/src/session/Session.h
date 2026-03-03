#ifndef SLINK_SESSION_H
#define SLINK_SESSION_H

#include <asio.hpp>

#include "message/Message.h"
#include "safe_queue/SafeQueue.h"
#include <utility/benchmark/Benchmark.h>

#include "../user/User.h"

namespace sLink::server::session
{
    class Session
    {
    private:
        static constexpr std::string_view s_BenchmarkOutputColor = SLINK_CL_CLR_RED;

    public:
        using OnLoginInfoSentCallback = std::move_only_function<void(const user::User&)>;

        using OnRegisterInfoSentCallback = std::move_only_function<void(const user::User&)>;

        using OnDisconnectCallback = std::move_only_function<void()>;

        Session(asio::ip::tcp::socket &&socket, utility::SafeQueue<std::string> &inbox);

        void start(const std::shared_ptr<Session> &self);

        void disconnect();

        void disconnectAfterWrite(const std::shared_ptr<Session>& self);

        void send(const message::Message &message, const std::shared_ptr<Session>& self);

        auto getUser(this auto&& self)
        {
            return self.m_User;
        }

        void setOnLoginInfoSentCallback(OnLoginInfoSentCallback &&callback);

        void setOnRegisterInfoSentCallback(OnRegisterInfoSentCallback &&callback);

        void setOnDisconnectCallback(OnDisconnectCallback &&callback);

        ~Session();

    private:
        void onRead(const std::shared_ptr<Session>& self);

        void onWrite(const std::shared_ptr<Session>& self);

        void handleMessage();

        user::User m_User;

        asio::ip::tcp::socket m_Socket;

        asio::streambuf m_Buffer;

        utility::SafeQueue<std::string> &m_Inbox;

        std::queue<std::string> m_WriteQueue;

        OnLoginInfoSentCallback m_OnLoginInfoSentCallback;

        OnRegisterInfoSentCallback m_OnRegisterInfoSentCallback;

        OnDisconnectCallback m_OnDisconnectCallback;

        bool m_ShouldDisconnectAfterWrite;
    };
}

#endif
