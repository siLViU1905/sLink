#include "Database.h"

#include <format>

namespace sLink::server::db
{
    Database::Database() : m_DatabaseHandle(nullptr), m_Closed(false)
    {
    }

    void Database::run(utility::SafeQueue<std::string> &rawMessageInbox)
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

            if (auto request = m_Requests.tryPop())
            {
                if (request->m_Type == UserRequest::RequestType::LOGIN)
                {
                    result = checkUserLoginInfo(request->m_User);

                    if (result)
                        m_Responses.push({
                            request->m_User.getUsername().data(), "", Response::ResponseType::LOGIN_SUCCESS
                        });
                    else
                        m_Responses.push({
                            request->m_User.getUsername().data(), result.error(), Response::ResponseType::LOGIN_FAIL
                        });
                } else if (request->m_Type == UserRequest::RequestType::REGISTER)
                {
                    result = checkUserRegisterInfo(request->m_User);

                    if (result)
                    {
                        if (auto addResult = addUser(request->m_User))
                            m_Responses.push({
                                request->m_User.getUsername().data(), "", Response::ResponseType::REGISTER_SUCCESS
                            });
                        else
                            m_Responses.push({
                                request->m_User.getUsername().data(), addResult.error(),
                                Response::ResponseType::REGISTER_FAIL
                            });
                    } else
                        m_Responses.push({
                            request->m_User.getUsername().data(), result.error(), Response::ResponseType::REGISTER_FAIL
                        });
                }

                m_InfoOutbox.push(result ? *result : result.error());
            }

            if (auto rawMessage = rawMessageInbox.tryPop())
            {
                result = addMessage(message::Message::deserialize(*rawMessage));

                m_InfoOutbox.push(result ? *result : result.error());
            }
        }
    }

    utility::SafeQueue<Database::Response> &Database::getUserResponses()
    {
        return m_Responses;
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

    Database::ActionResult Database::addUser(const user::User &user)
    {
        SLINK_START_BENCHMARK

        sqlite3_stmt *stmt;

        if (sqlite3_prepare_v2(m_DatabaseHandle, s_InsertUserQuery.data(), -1, &stmt, nullptr) != SQLITE_OK)
            return std::unexpected(std::format("Failed to prepare insert user query"));

        sqlite3_bind_text(stmt, 1, user.getUsername().data(), -1, SQLITE_TRANSIENT);

        sqlite3_bind_text(stmt, 2, user.getPassword().data(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) != SQLITE_DONE)
        {
            sqlite3_finalize(stmt);

            return std::unexpected(std::format("Failed to add user '{}'", user.getUsername()));
        }

        sqlite3_finalize(stmt);

        SLINK_END_BENCHMARK("[Database]", "addUser", s_BenchmarkOutputColor)

        return {std::format("User '{}' successfully added", user.getUsername())};
    }

    std::optional<int> Database::getUserId(const user::User &user) const
    {
        SLINK_START_BENCHMARK

        sqlite3_stmt *stmt;

        if (sqlite3_prepare_v2(m_DatabaseHandle, s_GetUserIdQuery.data(), -1, &stmt, nullptr) != SQLITE_OK)
            return std::nullopt;

        sqlite3_bind_text(stmt, 1, user.getUsername().data(), -1, SQLITE_TRANSIENT);

        std::optional<int> userId;

        if (sqlite3_step(stmt) == SQLITE_ROW)
            userId = sqlite3_column_int(stmt, 0);

        sqlite3_finalize(stmt);

        SLINK_END_BENCHMARK("[Database]", "getUserId", s_BenchmarkOutputColor)

        return userId;
    }

    bool Database::findUser(const user::User &user) const
    {
        return getUserId(user).has_value();
    }

    void Database::requestUserLogin(const user::User &user)
    {
        m_Requests.push({user, UserRequest::RequestType::LOGIN});
    }

    void Database::requestUserRegister(const user::User &user)
    {
        m_Requests.push({user, UserRequest::RequestType::REGISTER});
    }

    Database::ActionResult Database::checkUserLoginInfo(const user::User &user) const
    {
        SLINK_START_BENCHMARK

        if (auto userId = getUserId(user))
        {
            auto result = checkUserPassword(*userId, user);

            SLINK_END_BENCHMARK("[Database]", "checkUserLoginInfo", s_BenchmarkOutputColor);

            if (result)
                return {*result};

            return std::unexpected(result.error());
        }

        SLINK_END_BENCHMARK("[Database]", "checkUserLoginInfo", s_BenchmarkOutputColor);

        return std::unexpected(std::format("User {} could not be found", user.getUsername()));
    }

    Database::ActionResult Database::checkUserRegisterInfo(const user::User &user)
    {
        SLINK_START_BENCHMARK

        if (findUser(user))
        {
            SLINK_END_BENCHMARK("[Database]", "checkUserRegisterInfo", s_BenchmarkOutputColor);

            return std::unexpected(std::format("User {} already exist", user.getUsername()));
        }

        SLINK_END_BENCHMARK("[Database]", "checkUserRegisterInfo", s_BenchmarkOutputColor);

        return {std::format("User {} register info are good", user.getUsername())};
    }

    Database::ActionResult Database::checkUserPassword(int userId, const user::User &user) const
    {
        SLINK_START_BENCHMARK

        sqlite3_stmt *stmt;

        if (sqlite3_prepare_v2(m_DatabaseHandle, s_GetUserPasswordQuery.data(), -1, &stmt, nullptr) != SQLITE_OK)
            return std::unexpected("Failed to prepare get user password query");

        sqlite3_bind_int(stmt, 1, userId);

        bool passwordMatches = false;

        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            const unsigned char *storedPassword = sqlite3_column_text(stmt, 0);

            if (storedPassword != nullptr)
            {
                std::string stored = reinterpret_cast<const char *>(storedPassword);

                std::string provided = user.getPassword().data();

                passwordMatches = (provided == stored);
            }
        }

        sqlite3_finalize(stmt);

        SLINK_END_BENCHMARK("[Database]", "checkUserPassword", s_BenchmarkOutputColor)

        return passwordMatches
                   ? ActionResult{std::format("User {} password matches", user.getUsername())}
                   : std::unexpected(std::format("User {} password does not match", user.getUsername()));
    }

    Database::ActionResult Database::addMessage(const message::Message &message)
    {
        SLINK_START_BENCHMARK

        sqlite3_stmt *stmt;

        if (sqlite3_prepare_v2(m_DatabaseHandle, s_InsertMessageQuery.data(), -1, &stmt, nullptr) != SQLITE_OK)
            return std::unexpected(std::format("Failed to prepare insert message query"));

        if (auto userId = getUserId({message.getSenderName(), ""}))
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
