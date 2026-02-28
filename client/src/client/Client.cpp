#include "Client.h"

namespace sLink::client
{
    Client::Client(asio::io_context &ctx)
        : m_IOContext(ctx),
          m_WorkGuard(asio::make_work_guard(ctx)),
          m_Socket(ctx),
          m_ConnectionFailed(false)
    {
    }

    void Client::setUsername(std::string_view name)
    {
        m_Username = name;
    }

    void Client::setPassword(std::string_view password)
    {
        m_Password = password;
    }

    std::string_view Client::getUsername() const
    {
        return m_Username;
    }

    std::expected<std::string, std::string> Client::connect(std::string_view host, std::string_view port,
                                                            protocol::Command joinType)
    {
        asio::ip::tcp::resolver resolver(m_IOContext);

        auto endpoints = resolver.resolve(host, port);

        auto result = onConnect(endpoints, joinType);

        if (result)
            return {std::format("Connected to port {}", port)};

        return std::unexpected(std::format("Failed to connect to port {} -> {}", port, result.error()));
    }

    void Client::send(const message::Message &message)
    {
        auto serialized = message.serialize() + "\n";

        asio::post(m_IOContext, [this, msg = std::move(serialized)]() mutable
        {
            bool idle = m_WriteQueue.empty();

            m_WriteQueue.push(std::move(msg));

            if (idle)
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

    std::expected<std::string, std::string> Client::onConnect(asio::ip::tcp::resolver::results_type endpoints,
                                                              protocol::Command joinType)
    {
        auto promise = std::make_shared<std::promise<std::expected<std::string, std::string> > >();

        auto future = promise->get_future();

        asio::async_connect(m_Socket, endpoints,
                            [this, promise, joinType](std::error_code ec, asio::ip::tcp::endpoint)
                            {
                                if (!ec)
                                {
                                    onJoin(joinType);

                                    onRead();

                                    promise->set_value({});
                                } else
                                    promise->set_value(std::unexpected(ec.message()));
                            });

        return future.get();
    }

    void Client::onWrite()
    {
        SLINK_START_BENCHMARK

        asio::async_write(m_Socket, asio::buffer(m_WriteQueue.front()),
                          [this](std::error_code ec, size_t)
                          {
                              if (!ec)
                              {
                                  m_WriteQueue.pop();
                                  if (!m_WriteQueue.empty())
                                      onWrite();
                              } else
                                  m_Socket.close();
                          });

        SLINK_END_BENCHMARK("[CLIENT]", "onWrite", s_BenchmarkOutputColor)
    }

    void Client::onRead()
    {
        SLINK_START_BENCHMARK

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

        SLINK_END_BENCHMARK("[CLIENT]", "onRead", s_BenchmarkOutputColor)
    }

    void Client::onJoin(protocol::Command joinType)
    {
        message::Message joinMessage(joinType, m_Username, m_Password);

        auto data = joinMessage.serialize() + "\n";

        asio::write(m_Socket, asio::buffer(data));
    }
}
