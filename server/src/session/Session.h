#ifndef SLINK_SESSION_H
#define SLINK_SESSION_H

#include <asio.hpp>

#include "safe_queue/SafeQueue.h"

namespace sLink::session
{
    class Session : public std::enable_shared_from_this<Session>
    {
    public:
        using OnUsernameSentCallback = std::move_only_function<void(std::string_view)>;

        using OnDisconnectCallback = std::move_only_function<void(std::string_view)>;

        Session(asio::ip::tcp::socket &&socket, utility::SafeQueue<std::string> &inbox);

        void start();

        void disconnect();

        void send(const std::string &message);

        void setUsername(std::string_view username);

        std::string_view getUsername() const;

        void setOnUsernameSentCallback(OnUsernameSentCallback &&callback);

        void setOnDisconnectCallback(OnDisconnectCallback &&callback);

    private:
        void onRead();

        void onWrite();

        std::string m_Username;

        asio::ip::tcp::socket m_Socket;

        asio::streambuf m_Buffer;

        utility::SafeQueue<std::string> &m_Inbox;

        std::queue<std::string> m_WriteQueue;

        OnUsernameSentCallback m_OnUsernameSentCallback;

        OnDisconnectCallback m_OnDisconnectCallback;
    };
}

#endif