#ifndef SLINK_PROFILEPICTURE_H
#define SLINK_PROFILEPICTURE_H
#include <cmath>
#include <cstdint>
#include <expected>
#include <string>
#include <string_view>
#include <vector>

namespace sLink::profile_picture
{
    class ProfilePicture
    {
    public:
        static constexpr uint32_t s_ImageWidth = 64;
        static constexpr uint32_t s_ImageHeight = 64;
        static constexpr uint32_t s_ImageChannels = 4;
        static constexpr uint32_t s_ImageSize = s_ImageWidth * s_ImageHeight * s_ImageChannels;

        static const uint32_t s_MipLevels;

        using LoadResult = std::expected<std::string, std::string>;

        ProfilePicture();

        LoadResult loadImage(std::string_view path);

        auto getPixels(this auto&& self)
        {
            return self.m_Pixels;
        }

    private:
        std::vector<uint8_t> m_Pixels;

        uint32_t m_MipLevels;
    };
}

#endif
