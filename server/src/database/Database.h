#ifndef SLINK_DATABASE_H
#define SLINK_DATABASE_H
#include <string_view>

#include <sqlite3.h>
#include <memory>
#include <string>
#include <expected>

namespace sLink::server::db
{
    class Database
    {
    private:
        static constexpr std::string_view s_DatabaseName = "sLink_storage.db";

        static constexpr std::string_view s_CreateUsersTableQuery =
            "CREATE TABLE IF NOT EXISTS users ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "username VARCHAR(25) NOT NULL"
            ");";

        static constexpr std::string_view s_CreateMessagesTableQuery =
            "CREATE TABLE IF NOT EXISTS messages ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "content VARCHAR(255), "
            "user_id INTEGER, "
            "FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE"
            ");";

        static constexpr std::string_view s_InsertUserQuery = "INSERT INTO users (username) VALUES (?);";

    public:
        Database();

        std::expected<std::string, std::string> start();

        std::expected<std::string, std::string> addUser(std::string_view username);

        ~Database();

    private:
        sqlite3* m_DatabaseHandle;
    };
}

#endif //SLINK_DATABASE_H
