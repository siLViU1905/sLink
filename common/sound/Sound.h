#ifndef SLINK_SOUND_H
#define SLINK_SOUND_H

#include <expected>
#include <miniaudio.h>
#include <string>

namespace sLink::sound
{
    class Sound
    {
    public:
        using LoadResult = std::expected<void, std::string>;

        Sound();

        LoadResult load(std::string_view path);

        void setVolume(float volume);

        void play();

        ~Sound();

    private:
        ma_engine m_SoundEngine;

        ma_sound m_SoundHandle;
    };
}

#endif //SLINK_SOUND_H
