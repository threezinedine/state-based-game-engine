#include <NTTEngine/audio/audio.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/platforms/path.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <cstring>

#include "audio_platforms.hpp"

namespace ntt::audio
{
    using namespace memory;
    using namespace log;
    using namespace event;

#define MAX_AUDIO 100

    namespace
    {
        b8 s_isInitialized = FALSE;
        audio_id_t s_currentAudioId = DEFAULT_AUDIO;

        struct AudioInfo
        {
            SOUND sound;
            String path;
        };

        Scope<AudioInfo> s_loadedAudios[MAX_AUDIO];
        Dictionary<String, audio_id_t> s_loadedAudiosPath;
        List<audio_id_t> s_playingAudios;
    } // namespace

    void AudioInit()
    {
        if (s_isInitialized)
        {
            return;
        }

        memset(s_loadedAudios, 0, sizeof(s_loadedAudios));
        s_loadedAudiosPath = {};
        s_playingAudios = {};

        INIT_DEVICE();

        LoadAudio(RelativePath("assets/audios/default.wav"));

        s_isInitialized = TRUE;
    }

    audio_id_t LoadAudio(const String &path)
    {
        if (!s_isInitialized)
        {
            return DEFAULT_AUDIO;
        }

        if (s_loadedAudiosPath.Contains(path))
        {
            NTT_ENGINE_WARN("The audio file is already loaded: {}", GetFileName(path, true));
            return s_loadedAudiosPath.Get(path);
        }

        LOAD_SOUND(sound, path);

        if (IS_LOADED_SUCCESS(sound))
        {
            s_loadedAudios[s_currentAudioId] = CreateScope<AudioInfo>();
            s_loadedAudios[s_currentAudioId]->sound = sound;
            s_loadedAudios[s_currentAudioId]->path = path;
            s_loadedAudiosPath.Insert(path, s_currentAudioId);
        }
        else
        {
            NTT_ENGINE_WARN("Loading Audio {} Error", GetFileName(path, true));
            return DEFAULT_AUDIO;
        }

        return s_currentAudioId++;
    }

    void PlayAudio(audio_id_t audio_id)
    {
        if (!s_isInitialized)
        {
            return;
        }

        if (s_loadedAudios[audio_id] == nullptr)
        {
            NTT_ENGINE_WARN("The audio id is not existed or unloaded");
            return;
        }

        s_playingAudios.RemoveItem(audio_id);
        s_playingAudios.Add(audio_id);

        PLAY_SOUND(s_loadedAudios[audio_id]->sound);
    }

    void StopAudio(audio_id_t audio_id)
    {
        if (!s_isInitialized)
        {
            return;
        }

        if (s_loadedAudios[audio_id] == nullptr)
        {
            NTT_ENGINE_WARN("The audio id is not existed or unloaded");
            return;
        }

        if (IS_SOUND_PLAYING(s_loadedAudios[audio_id]->sound))
        {
            STOP_SOUND(s_loadedAudios[audio_id]->sound);
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
            if (!IS_SOUND_PLAYING(s_loadedAudios[s_playingAudios[i]]->sound))
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

        if (s_loadedAudios[audio_id] == nullptr)
        {
            NTT_ENGINE_WARN("The audio id is not existed or unloaded");
            return;
        }

        StopAudio(audio_id);
        UNLOAD_SOUND(s_loadedAudios[audio_id]->sound);
        s_loadedAudiosPath.Remove(s_loadedAudios[audio_id]->path);
        s_loadedAudios[audio_id] = nullptr;
    }

    void AudioShutdown()
    {
        if (!s_isInitialized)
        {
            return;
        }

        for (u32 i = 0; i < MAX_AUDIO; i++)
        {
            if (s_loadedAudios[i] != nullptr)
            {
                NTT_ENGINE_WARN("The audio id {} is not unloaded, unloading...", i);
                UnloadAudio(i);
            }
        }

        s_isInitialized = FALSE;
    }
} // namespace ntt::audio
