#include "Database.h"

#include <format>

namespace sLink::server::db
{
    Database::Database() : m_DatabaseHandle(nullptr)
    {
    }

    std::expected<std::string, std::string> Database::start()
    {
        if (sqlite3_open(s_DatabaseName.data(), &m_DatabaseHandle) != SQLITE_OK)
            return std::unexpected(std::format("Failed to start database {}", s_DatabaseName));

        char *err;

        if (sqlite3_exec(m_DatabaseHandle, s_CreateUsersTableQuery.data(), nullptr, nullptr, &err) != SQLITE_OK)
            return std::unexpected(std::format("Failed to create users table. Error: {}", err));

        if (sqlite3_exec(m_DatabaseHandle, s_CreateMessagesTableQuery.data(), nullptr, nullptr, &err) != SQLITE_OK)
            return std::unexpected(std::format("Failed to create messages table. Error: {}", err));

        return {"Database successfully started"};
    }

    std::expected<std::string, std::string> Database::addUser(std::string_view username)
    {
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

        return {std::format("User '{}' successfully added", username)};
    }

    Database::~Database()
    {
        if (m_DatabaseHandle)
            sqlite3_close(m_DatabaseHandle);
    }
}
