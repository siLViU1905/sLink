#ifndef SLINK_BASE64_H
#define SLINK_BASE64_H
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace sLink::utility::base64
{
    static constexpr std::string_view s_Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string encode(std::span<const uint8_t> data);

    std::vector<uint8_t> decode(std::string_view in);
}

#endif //SLINK_BASE64_H
