#ifndef SLINK_PASSWORDHASHER_H
#define SLINK_PASSWORDHASHER_H
#include <cstdint>
#include <string>

namespace sLink::server::db::password_hasher
{
    class PasswordHasher
    {
    private:
        static constexpr uint32_t s_Arbitrary32BitConst1 = 0x6A09E667;
        static constexpr uint32_t s_Arbitrary32BitConst2 = 0xBB67AE85;
        static constexpr uint32_t s_Arbitrary32BitConst3 = 0x3C6EF372;
        static constexpr uint32_t s_Arbitrary32BitConst4 = 0xA54FF53A;

        static constexpr uint32_t s_LargePrime = 0x9E3779B1;

        static constexpr uint32_t s_Shift1 = 7;
        static constexpr uint32_t s_Shift2 = 11;
        static constexpr uint32_t s_Shift3 = 13;
        static constexpr uint32_t s_Shift4 = 17;
        static constexpr uint32_t s_MixShift = 5;

        static uint32_t rotate_left(uint32_t val, int shift);

    public:
        static std::string hash(std::string_view password);
    };
}

#endif
