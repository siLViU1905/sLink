#ifndef SLINK_USER_H
#define SLINK_USER_H
#include <string_view>

namespace sLink::server::user
{
    class User
    {
    public:
        User() = default;

        User(std::string_view username, std::string_view password);

        void setUsername(std::string_view username);

        void setPassword(std::string_view password);

        std::string_view getUsername() const;

        std::string_view getPassword() const;

    private:
        std::string_view m_Username;

        std::string_view m_Password;
    };
}

#endif //SLINK_USER_H
