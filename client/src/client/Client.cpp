#include "Client.h"

namespace sLink::client
{
    Client::Client(asio::io_context &ctx)
        : m_IOContext(ctx),
          m_WorkGuard(asio::make_work_guard(ctx)),
          m_Socket(ctx),
          m_IsWriting(false),
          m_ConnectionFailed(false)
    {
    }

    void Client::setUsername(std::string_view name)
    {
        m_Username = name;
    }

    std::string_view Client::getUsername() const
    {
        return m_Username;
    }

    std::expected<std::string, std::string> Client::connect(std::string_view host, std::string_view port)
    {
        asio::ip::tcp::resolver resolver(m_IOContext);

        auto endpoints = resolver.resolve(host, port);

        auto result = onConnect(endpoints);

        if (result)
            return {std::format("Connected to port {}", port)};

        return std::unexpected(std::format("Failed to connect to port {} -> {}", port, result.error()));
    }

    void Client::send(const message::Message &message)
    {
        m_Outbox.push(message.serialize());

        asio::post(m_IOContext, [this]()
        {
            if (!m_IsWriting)
                onWrite();
        });
    }

    bool Client::isConnected() const
    {
        return m_Socket.is_open();
    }

    utility::SafeQueue<std::string> &Client::getInbox()
    {
        return m_Inbox;
    }

    std::expected<std::string, std::string> Client::onConnect(asio::ip::tcp::resolver::results_type endpoints)
    {
        auto promise = std::make_shared<std::promise<std::expected<std::string, std::string> > >();

        auto future = promise->get_future();

        asio::async_connect(m_Socket, endpoints,
                            [this, promise](std::error_code ec, asio::ip::tcp::endpoint)
                            {
                                if (!ec)
                                {
                                    onJoin();

                                    onRead();
                                } else
                                    promise->set_value(std::unexpected(ec.message()));
                            });

        return future.get();
    }

    void Client::onWrite()
    {
        auto msg = m_Outbox.tryPop();

        if (!msg)
        {
            m_IsWriting = false;
            return;
        }

        m_IsWriting = true;

        m_CurrentMessage = *msg + "\n";

        asio::async_write(m_Socket, asio::buffer(m_CurrentMessage),
                          [this](std::error_code ec, size_t)
                          {
                              if (!ec)
                                  onWrite();
                              else
                              {
                                  m_IsWriting = false;
                                  m_Socket.close();
                              }
                          });
    }

    void Client::onRead()
    {
        asio::async_read_until(m_Socket, m_ReadBuffer, '\n',
                               [this](std::error_code ec, size_t length)
                               {
                                   if (!ec)
                                   {
                                       std::istream is(&m_ReadBuffer);

                                       std::string line;

                                       std::getline(is, line);

                                       m_Inbox.push(std::move(line));

                                       onRead();
                                   } else
                                       m_Socket.close();
                               });
    }

    void Client::onJoin()
    {
        auto joinMessage = "/" + m_Username + "\n";

        asio::write(m_Socket, asio::buffer(joinMessage));
    }
}
