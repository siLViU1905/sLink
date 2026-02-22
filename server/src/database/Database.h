#ifndef SLINK_DATABASE_H
#define SLINK_DATABASE_H
#include <string_view>

#include <sqlite3.h>
#include <memory>
#include <string>
#include <expected>

#include "message/Message.h"
#include "safe_queue/SafeQueue.h"
#include <utility/benchmark/Benchmark.h>

#include "../user/User.h"

namespace sLink::server::db
{
    class Database
    {
    private:
        static constexpr std::string_view s_DatabaseName = "sLink_storage.db";

        static constexpr std::string_view s_CreateUsersTableQuery =
                "CREATE TABLE IF NOT EXISTS users ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "username VARCHAR(25) NOT NULL,"
                "password VARCHAR(32) NOT NULL"
                ");";

        static constexpr std::string_view s_CreateMessagesTableQuery =
                "CREATE TABLE IF NOT EXISTS messages ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "content VARCHAR(255), "
                "timestamp BIGINT, "
                "user_id INTEGER, "
                "FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE"
                ");";

        static constexpr std::string_view s_InsertUserQuery = "INSERT INTO users (username, password) VALUES (?, ?);";

        static constexpr std::string_view s_GetUserIdQuery = "SELECT id FROM users WHERE username = ?;";

        static constexpr std::string_view s_GetUserPasswordQuery = "SELECT password FROM users WHERE id = ?;";

        static constexpr std::string_view s_InsertMessageQuery =
                "INSERT INTO messages (content, timestamp, user_id) VALUES (?, ?, ?);";

        static constexpr std::string_view s_BenchmarkOutputColor = SLINK_CL_CLR_YELLOW;

    public:
        struct Response
        {
            enum class ResponseType
            {
                LOGIN_SUCCESS,
                LOGIN_FAIL,
                REGISTER_SUCCESS,
                REGISTER_FAIL
            };

            std::string m_Username;

            std::string m_Message;

            ResponseType m_Type;
        };

        Database();

        void run(utility::SafeQueue<std::string> &rawMessageInbox);

        utility::SafeQueue<Response> &getUserResponses();

        utility::SafeQueue<std::string> &getInfo();

        bool findUser(const user::User &user) const;

        void requestUserLogin(const user::User &user);

        void requestUserRegister(const user::User &user);

        void close();

        ~Database();

    private:
        struct UserRequest
        {
            enum class RequestType
            {
                LOGIN,
                REGISTER
            };

            user::User m_User;

            RequestType m_Type;
        };


        using ActionResult = std::expected<std::string, std::string>;

        ActionResult start();

        ActionResult addUser(const user::User &user);

        ActionResult addMessage(const message::Message &message);

        std::optional<int> getUserId(const user::User &user) const;

        ActionResult checkUserLoginInfo(const user::User &user) const;

        ActionResult checkUserRegisterInfo(const user::User &user);

        ActionResult checkUserPassword(int userId, const user::User &user) const;

        sqlite3 *m_DatabaseHandle;

        utility::SafeQueue<UserRequest> m_Requests;

        utility::SafeQueue<Response> m_Responses;

        utility::SafeQueue<std::string> m_InfoOutbox;

        std::mutex m_CloseMutex;

        bool m_Closed;
    };
}

#endif //SLINK_DATABASE_H
