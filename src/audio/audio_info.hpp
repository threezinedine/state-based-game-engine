#pragma once
#include <NTTEngine/defines.hpp>
#include "audio_platforms.hpp"
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/audio/audio.hpp>

namespace ntt::audio
{
    struct AudioInfo
    {
        SOUND sound;
        String path;
    };
};