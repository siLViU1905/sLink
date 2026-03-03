#include "ProfilePicture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <format>
#include <stb_image_resize2.h>

namespace sLink::client::profile_picture
{
    const uint32_t ProfilePicture::s_MipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(s_ImageWidth, s_ImageHeight)))) + 1;

    ProfilePicture::ProfilePicture():m_MipLevels(0)
    {
    }

    ProfilePicture::LoadResult ProfilePicture::loadImage(std::string_view path)
    {
        int width, height, channels;
        if (auto *pixels = stbi_load(path.data(), &width, &height, &channels, STBI_rgb_alpha))
        {
            m_Pixels.resize(s_ImageSize);
            if (!stbir_resize(pixels, width, height, 0,
                              m_Pixels.data(), s_ImageWidth, s_ImageHeight,
                              0,
                              STBIR_RGBA, STBIR_TYPE_UINT8, STBIR_EDGE_CLAMP, STBIR_FILTER_DEFAULT))
                return std::unexpected(std::format("Failed to resize image with path: {}", path));

            return {"Image loaded and resized successfully"};
        }

        return std::unexpected(std::format("Failed to load image with path: {}", path));
    }
}
