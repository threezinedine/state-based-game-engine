#include <NTTEngine/audio/audio.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/platforms/path.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <cstring>
#include <NTTEngine/dev/store.hpp>
#include "audio_info.hpp"
#include <NTTEngine/dev/store.hpp>

#include "audio_platforms.hpp"

namespace ntt::audio
{
    using namespace memory;
    using namespace log;
    using namespace event;
    using namespace dev::store;

#define MAX_AUDIO 100

    namespace
    {
        b8 s_isInitialized = FALSE;

        Store<audio_id_t, AudioInfo, String> s_audioStore(DEFAULT_AUDIO,
                                                          MAX_AUDIO, [](const AudioInfo &audio)
                                                          { return audio.path; });

        List<audio_id_t> s_playingAudios;
    } // namespace

    void AudioInit()
    {
        if (s_isInitialized)
        {
            return;
        }

        s_playingAudios = {};

        INIT_DEVICE();

        // LoadAudio(RelativePath("assets/audios/default.wav"));

        s_isInitialized = TRUE;
    }

    audio_id_t LoadAudio(const String &path)
    {
        if (!s_isInitialized)
        {
            return DEFAULT_AUDIO;
        }

        if (s_audioStore.ContainsUnique(path))
        {
            NTT_ENGINE_WARN("The audio file is already loaded: {}", GetFileName(path, true));
            return DEFAULT_AUDIO;
        }

        LOAD_SOUND(sound, path);

        if (IS_LOADED_SUCCESS(sound))
        {
            AudioInfo info = {sound, path};
            return s_audioStore.Add(info);
        }
        else
        {
            NTT_ENGINE_WARN("Loading Audio {} Error", GetFileName(path, true));
            return DEFAULT_AUDIO;
        }
    }

    void PlayAudio(audio_id_t audio_id)
    {
        if (!s_isInitialized)
        {
            return;
        }

        auto audioInfo = s_audioStore.Get(audio_id);

        if (audioInfo == nullptr)
        {
            NTT_ENGINE_WARN("The audio id is not existed or unloaded");
            return;
        }

        s_playingAudios.RemoveItem(audio_id);
        s_playingAudios.Add(audio_id);
        PLAY_SOUND(audioInfo->sound);
    }

    void StopAudio(audio_id_t audio_id)
    {
        if (!s_isInitialized)
        {
            return;
        }

        auto audioInfo = s_audioStore.Get(audio_id);

        if (audioInfo == nullptr)
        {
            NTT_ENGINE_WARN("The audio id is not existed or unloaded");
            return;
        }

        if (IS_SOUND_PLAYING(audioInfo->sound))
        {
            STOP_SOUND(audioInfo->sound);
        }
    }

    void AudioUpdate(f32 delta)
    {
        if (!s_isInitialized)
        {
            return;
        }

        for (u32 i = 0; i < s_playingAudios.Length(); i++)
        {
            auto audioInfo = s_audioStore.Get(s_playingAudios[i]);
            if (!IS_SOUND_PLAYING(audioInfo->sound))
            {
                EventContext context;
                context.u32_data[0] = s_playingAudios[i];
                s_playingAudios.Remove(i);
                TriggerEvent(EventCode::AUDIO_FINISHED, nullptr, context);
            }
        }
    }

    void UnloadAudio(audio_id_t audio_id)
    {
        if (!s_isInitialized)
        {
            return;
        }

        if (audio_id == DEFAULT_AUDIO)
        {
            NTT_ENGINE_WARN("The default audio is not allowed to unload");
            return;
        }

        auto audioInfo = s_audioStore.Get(audio_id);

        if (audioInfo == nullptr)
        {
            NTT_ENGINE_WARN("The audio id is not existed or unloaded");
            return;
        }

        StopAudio(audio_id);
        UNLOAD_SOUND(audioInfo->sound);
        s_audioStore.Release(audio_id);
    }

    void AudioShutdown()
    {
        if (!s_isInitialized)
        {
            return;
        }

        auto availableIds = s_audioStore.GetAvailableIds();

        for (auto i = 0; i < availableIds.Length(); i++)
        {
            UnloadAudio(availableIds[i]);
        }

        s_isInitialized = FALSE;
    }
} // namespace ntt::audio
