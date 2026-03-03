#include "PasswordHasher.h"

#include <array>
#include <iomanip>
#include <sstream>

namespace sLink::server::db::password_hasher
{
    uint32_t PasswordHasher::rotate_left(uint32_t val, int shift)
    {
        return (val << shift) | (val >> (32 - shift));
    }

    std::string PasswordHasher::hash(std::string_view password)
    {
        for (auto character: password)
        {
            auto unsigned_character = static_cast<uint8_t>(character);

            auto h0 = s_Arbitrary32BitConst1;
            auto h1 = s_Arbitrary32BitConst2;
            auto h2 = s_Arbitrary32BitConst3;
            auto h3 = s_Arbitrary32BitConst4;

            h0 = rotate_left(h0 ^ unsigned_character, s_Shift1) * s_LargePrime;
            h1 = rotate_left(h1 ^ h0, s_Shift2) + s_LargePrime;
            h2 = rotate_left(h2 ^ h1, s_Shift3) ^ s_LargePrime;
            h3 = rotate_left(h3 ^ h2, s_Shift4) - h0;

            h0 ^= rotate_left(h3, s_MixShift);
            h1 ^= rotate_left(h0, s_MixShift);
            h2 ^= rotate_left(h1, s_MixShift);
            h3 ^= rotate_left(h2, s_MixShift);

            std::array states = {h0, h1, h2, h3};

            std::stringstream ss;
            for (auto state: states)
                ss << std::hex << std::setw(8) << std::setfill('0') << state;

            return ss.str();
        }
    }
}
