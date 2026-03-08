#include "Sound.h"

#include <format>

namespace sLink::sound
{
    Sound::Sound():m_SoundEngine(), m_SoundHandle()
    {
        ma_engine_init(nullptr, &m_SoundEngine);
    }

    Sound::LoadResult Sound::load(std::string_view path)
    {
        auto result = ma_sound_init_from_file(
            &m_SoundEngine,
            path.data(),
            0,
            nullptr,
            nullptr,
            &m_SoundHandle
            );

        if (result != MA_SUCCESS)
            return std::unexpected(std::format("Failed to load sound with path: {}", path));

        return {};
    }

    void Sound::setVolume(float volume)
    {
        ma_sound_set_volume(&m_SoundHandle, volume);
    }

    void Sound::play()
    {
        ma_sound_start(&m_SoundHandle);
    }

    Sound::~Sound()
    {
        ma_sound_uninit(&m_SoundHandle);
        ma_engine_uninit(&m_SoundEngine);
    }
}
