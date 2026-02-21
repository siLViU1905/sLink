#include "Database.h"

#include <format>

namespace sLink::server::db
{
    Database::Database() : m_DatabaseHandle(nullptr), m_Closed(false)
    {
    }

    void Database::run(utility::SafeQueue<std::string> &usernameInbox, utility::SafeQueue<std::string>& rawMessageInbox)
    {
        auto result = start();

        m_InfoOutbox.push(result ? *result : result.error());

        if (!result)
            return;

        while (true)
        {
            {
                std::scoped_lock lock(m_CloseMutex);
                if (m_Closed)
                    break;
            }

            if (auto username = usernameInbox.tryPop())
            {
                result = addUser(*username);

                m_InfoOutbox.push(result ? *result : result.error());
            }

            if (auto rawMessage = rawMessageInbox.tryPop())
            {
                result = addMessage(message::Message::deserialize(*rawMessage));

                m_InfoOutbox.push(result ? *result : result.error());
            }
        }
    }

    utility::SafeQueue<std::string> &Database::getInfo()
    {
        return m_InfoOutbox;
    }

    void Database::close()
    {
        std::scoped_lock lock(m_CloseMutex);
        m_Closed = true;
    }

    Database::ActionResult Database::start()
    {
        SLINK_START_BENCHMARK

        if (sqlite3_open(s_DatabaseName.data(), &m_DatabaseHandle) != SQLITE_OK)
            return std::unexpected(std::format("Failed to start database {}", s_DatabaseName));

        char *err;

        if (sqlite3_exec(m_DatabaseHandle, s_CreateUsersTableQuery.data(), nullptr, nullptr, &err) != SQLITE_OK)
            return std::unexpected(std::format("Failed to create users table. Error: {}", err));

        if (sqlite3_exec(m_DatabaseHandle, s_CreateMessagesTableQuery.data(), nullptr, nullptr, &err) != SQLITE_OK)
            return std::unexpected(std::format("Failed to create messages table. Error: {}", err));

        SLINK_END_BENCHMARK("[Database]", "start", s_BenchmarkOutputColor)

        return {"Database successfully started"};
    }

    Database::ActionResult Database::addUser(std::string_view username)
    {
        SLINK_START_BENCHMARK

        sqlite3_stmt *stmt;

        if (sqlite3_prepare_v2(m_DatabaseHandle, s_InsertUserQuery.data(), -1, &stmt, nullptr) != SQLITE_OK)
            return std::unexpected(std::format("Failed to prepare insert user query"));

        sqlite3_bind_text(stmt, 1, username.data(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) != SQLITE_DONE)
        {
            sqlite3_finalize(stmt);

            return std::unexpected(std::format("Failed to add user '{}'", username));
        }

        sqlite3_finalize(stmt);

        SLINK_END_BENCHMARK("[Database]", "addUser", s_BenchmarkOutputColor)

        return {std::format("User '{}' successfully added", username)};
    }

    std::optional<int> Database::getUserId(std::string_view username) const
    {
        SLINK_START_BENCHMARK

        sqlite3_stmt *stmt;

        if (sqlite3_prepare_v2(m_DatabaseHandle, s_GetUserIdQuery.data(), -1, &stmt, nullptr) != SQLITE_OK)
            return std::nullopt;

        sqlite3_bind_text(stmt, 1, username.data(), -1, SQLITE_TRANSIENT);

        std::optional<int> userId;

        if (sqlite3_step(stmt) == SQLITE_ROW)
            userId = sqlite3_column_int(stmt, 0);

        sqlite3_finalize(stmt);

        SLINK_END_BENCHMARK("[Database]", "getUserId", s_BenchmarkOutputColor)

        return userId;
    }

    bool Database::findUser(std::string_view username) const
    {
        return getUserId(username).has_value();
    }

    Database::ActionResult Database::addMessage(const message::Message &message)
    {
        SLINK_START_BENCHMARK

        sqlite3_stmt *stmt;

        if (sqlite3_prepare_v2(m_DatabaseHandle, s_InsertMessageQuery.data(), -1, &stmt, nullptr) != SQLITE_OK)
            return std::unexpected(std::format("Failed to prepare insert message query"));

        if (auto userId = getUserId(message.getSenderName()))
        {
            sqlite3_bind_text(stmt, 1, message.getContent().data(), -1, SQLITE_TRANSIENT);

            sqlite3_bind_int64(stmt, 2, message.getTimestamp().getMs());

            sqlite3_bind_int(stmt, 3, *userId);
        } else
            return std::unexpected(std::format("Sender {} not found", message.getSenderName()));

        if (sqlite3_step(stmt) != SQLITE_DONE)
        {
            sqlite3_finalize(stmt);

            return std::unexpected("Failed to add message");
        }

        sqlite3_finalize(stmt);

        SLINK_END_BENCHMARK("[Database]", "addMessage", s_BenchmarkOutputColor)

        return {"Message successfully added"};
    }

    Database::~Database()
    {
        if (m_DatabaseHandle)
            sqlite3_close(m_DatabaseHandle);
    }
}
