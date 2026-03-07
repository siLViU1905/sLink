#include "Session.h"

#include "message/Message.h"

namespace sLink::server::session
{
    Session::Session(asio::ip::tcp::socket &&socket, utility::SafeQueue<std::string> &inbox)
        : m_Socket(std::move(socket)), m_Inbox(inbox), m_ShouldDisconnectAfterWrite(false)
    {
    }

    void Session::start(const std::shared_ptr<Session> &self)
    {
        onRead(self);
    }

    void Session::disconnect()
    {
        std::error_code ec;

        m_Socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);

        m_Socket.close(ec);
    }

    void Session::disconnectAfterWrite(const std::shared_ptr<Session>& self)
    {
        asio::post(m_Socket.get_executor(), [this, self]()
        {
            m_ShouldDisconnectAfterWrite = true;

            if (m_WriteQueue.empty())
                disconnect();
        });
    }

    void Session::send(const message::Message &message, const std::shared_ptr<Session>& self)
    {
        SLINK_START_BENCHMARK

        asio::post(m_Socket.get_executor(), [this, self, message]()
        {
            bool idle = m_WriteQueue.empty();

            m_WriteQueue.push(message.serialize() + "\n");

            if (idle)
                onWrite(self);
        });

        SLINK_END_BENCHMARK("[Session]", "send", s_BenchmarkOutputColor)
    }

    void Session::setOnLoginInfoSentCallback(OnLoginInfoSentCallback &&callback)
    {
        m_OnLoginInfoSentCallback = std::move(callback);
    }

    void Session::setOnRegisterInfoSentCallback(OnRegisterInfoSentCallback &&callback)
    {
        m_OnRegisterInfoSentCallback = std::move(callback);
    }

    void Session::setOnProfilePictureSentCallback(OnProfilePictureSentCallback &&callback)
    {
        m_OnProfilePictureSentCallback = std::move(callback);
    }

    void Session::setOnDisconnectCallback(OnDisconnectCallback &&callback)
    {
        m_OnDisconnectCallback = std::move(callback);
    }

    Session::~Session()
    {
        disconnect();
    }

    void Session::onRead(const std::shared_ptr<Session>& self)
    {
        asio::async_read_until(m_Socket, m_Buffer, '\n',
                               [this, self](std::error_code ec, size_t)
                               {
                                   if (!ec)
                                   {
                                       handleMessage();

                                       onRead(self);
                                   } else if (ec == asio::error::eof || ec == asio::error::connection_reset)
                                   {
                                       if (m_OnDisconnectCallback)
                                           m_OnDisconnectCallback();
                                   }
                               });
    }

    void Session::onWrite(const std::shared_ptr<Session>& self)
    {
        asio::async_write(m_Socket, asio::buffer(m_WriteQueue.front()),
                          [this, self](std::error_code ec, size_t)
                          {
                              if (!ec)
                              {
                                  m_WriteQueue.pop();

                                  if (!m_WriteQueue.empty())
                                      onWrite(self);
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
                    m_User = {message.getSenderName(), message.getContent()};

                    m_OnLoginInfoSentCallback(m_User);
                    break;
                case protocol::Command::REGISTER_REQUEST:
                    m_User = {message.getSenderName(), message.getContent()};

                    m_OnRegisterInfoSentCallback(m_User);
                    break;
                case protocol::Command::CHAT_MESSAGE:
                    m_Inbox.push(line);

                    break;
                case protocol::Command::PROFILE_PICTURE:
                    m_OnProfilePictureSentCallback(m_User, message.getContent());

                    m_Inbox.push(line);
                    break;
               default:
                    break;
            }
        }

        SLINK_END_BENCHMARK("[Session]", "handleMessage", s_BenchmarkOutputColor)
    }
}
