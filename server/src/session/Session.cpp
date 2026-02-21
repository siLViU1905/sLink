#include "Session.h"

#include "message/Message.h"
#include <utility/benchmark/Benchmark.h>

namespace sLink::session
{
    Session::Session(asio::ip::tcp::socket &&socket, utility::SafeQueue<std::string> &inbox)
        : m_Socket(std::move(socket)), m_Inbox(inbox), m_ShouldDisconnectAfterWrite(false)
    {
    }

    void Session::start()
    {
        onRead();
    }

    void Session::disconnect()
    {
        std::error_code ec;

        m_Socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);

        m_Socket.close(ec);
    }

    void Session::disconnectAfterWrite()
    {
        auto self(shared_from_this());
        asio::post(m_Socket.get_executor(), [this, self]()
        {
            m_ShouldDisconnectAfterWrite = true;

            if (m_WriteQueue.empty())
                disconnect();
        });
    }

    void Session::send(const message::Message &message)
    {
        SLINK_START_BENCHMARK

        auto self(shared_from_this());

        asio::post(m_Socket.get_executor(), [this, self, message]()
        {
            bool idle = m_WriteQueue.empty();

            m_WriteQueue.push(message.serialize() + "\n");

            if (idle)
                onWrite();
        });

        SLINK_END_BENCHMARK("[Session]", "send")
    }

    void Session::setUsername(std::string_view username)
    {
        m_Username = username;
    }

    std::string_view Session::getUsername() const
    {
        return m_Username;
    }

    void Session::setOnUsernameSentCallback(OnUsernameSentCallback &&callback)
    {
        m_OnUsernameSentCallback = std::move(callback);
    }

    void Session::setOnDisconnectCallback(OnDisconnectCallback &&callback)
    {
        m_OnDisconnectCallback = std::move(callback);
    }

    void Session::onRead()
    {
        auto self(shared_from_this());

        asio::async_read_until(m_Socket, m_Buffer, '\n',
                               [this, self](std::error_code ec, size_t)
                               {
                                   if (!ec)
                                   {
                                       handleMessage();

                                       onRead();
                                   } else if (ec == asio::error::eof || ec == asio::error::connection_reset)
                                   {
                                       if (m_OnDisconnectCallback)
                                           m_OnDisconnectCallback(m_Username);
                                   }
                               });
    }

    void Session::onWrite()
    {
        auto self(shared_from_this());

        asio::async_write(m_Socket, asio::buffer(m_WriteQueue.front()),
                          [this, self](std::error_code ec, size_t)
                          {
                              if (!ec)
                              {
                                  m_WriteQueue.pop();

                                  if (!m_WriteQueue.empty())
                                      onWrite();
                                  else if (m_ShouldDisconnectAfterWrite)
                                      disconnect();
                              }
                          });
    }

    void Session::handleMessage()
    {
        SLINK_START_BENCHMARK

        std::istream is(&m_Buffer);

        std::string line;

        while (std::getline(is, line))
        {
            auto message = message::Message::deserialize(line);

            switch (message.getCommand())
            {
                case protocol::Command::LOGIN_REQUEST:
                    m_Username = message.getSenderName();

                    m_OnUsernameSentCallback(m_Username);
                    break;
                case protocol::Command::LOGIN_RESPONSE_REJECT:
                    break;
                case protocol::Command::LOGIN_RESPONSE_ACCEPT:
                    break;
                case protocol::Command::CHAT_MESSAGE:
                    send(message);
                    break;
                case protocol::Command::USER_JOINED:
                    break;
                case protocol::Command::USER_LEFT:
                    break;
            }
        }

        SLINK_END_BENCHMARK("[Session]", "handleMessage")
    }
}
