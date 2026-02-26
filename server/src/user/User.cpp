#include "User.h"

namespace sLink::server::user
{
    User::User(std::string_view username, std::string_view password) : m_Username(username), m_Password(password)
    {
    }

    void User::setUsername(std::string_view username)
    {
        m_Username = username;
    }

    void User::setPassword(std::string_view password)
    {
        m_Password = password;
    }

    std::string_view User::getUsername() const
    {
        return m_Username;
    }

    std::string_view User::getPassword() const
    {
        return m_Password;
    }
}
