#ifndef SLINK_DATABASE_H
#define SLINK_DATABASE_H
#include <string_view>

#include <sqlite3.h>
#include <memory>
#include <string>
#include <expected>

#include "message/Message.h"
#include "safe_queue/SafeQueue.h"

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
            "timestamp BIGINT, "
            "user_id INTEGER, "
            "FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE"
            ");";

        static constexpr std::string_view s_InsertUserQuery = "INSERT INTO users (username) VALUES (?);";

        static constexpr std::string_view s_GetUserIdQuery = "SELECT id FROM users WHERE username = ?;";

        static constexpr std::string_view s_InsertMessageQuery = "INSERT INTO messages (content, timestamp, user_id) VALUES (?, ?, ?);";

    public:
        Database();

        void run(utility::SafeQueue<std::string>& usernameInbox, utility::SafeQueue<std::string>& rawMessageInbox);

        utility::SafeQueue<std::string>& getInfo();

        bool findUser(std::string_view username) const;

        void close();

        ~Database();

    private:
        using ActionResult = std::expected<std::string, std::string>;

        ActionResult start();

        ActionResult addUser(std::string_view username);

        std::optional<int> getUserId(std::string_view username) const;

        ActionResult addMessage(const message::Message& message);

        sqlite3* m_DatabaseHandle;

        utility::SafeQueue<std::string> m_InfoOutbox;

        std::mutex m_CloseMutex;

        bool m_Closed;
    };
}

#endif //SLINK_DATABASE_H
