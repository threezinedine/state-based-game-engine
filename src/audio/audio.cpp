#include <NTTEngine/audio/audio.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/structures/dictionary.hpp>
#include <NTTEngine/structures/list.hpp>
#include <NTTEngine/platforms/path.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>
#include <cstring>
#include <NTTEngine/dev/store.hpp>
#include <NTTEngine/dev/store.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/resources/ResourceManager.hpp>
#include <NTTEngine/core/object.hpp>

#include "audio_platforms.hpp"

namespace ntt::audio
{
    using namespace memory;
    using namespace log;
    using namespace event;
    using namespace dev::store;

#define MAX_AUDIO 100

    struct AudioInfo : public Object
    {
        SOUND sound;
        String path;

        AudioInfo(SOUND sound, const String &path) : sound(sound), path(path) {}
    };

    struct PlayingAudioInfo
    {
        resource_id_t id;
        AudioContext context;
        u32 playedTimes = 0;

        PlayingAudioInfo(resource_id_t id)
            : id(id), context(AudioContext()), playedTimes(0) {}

        PlayingAudioInfo(resource_id_t id, const AudioContext &context)
            : id(id), context(context), playedTimes(0) {}

        PlayingAudioInfo(const PlayingAudioInfo &audio)
            : id(audio.id), context(audio.context), playedTimes(audio.playedTimes) {}

        b8 operator==(const PlayingAudioInfo &audio) const
        {
            return id == audio.id;
        }
    };

    namespace
    {
        b8 s_isInitialized = FALSE;

        Store<resource_id_t, AudioInfo> s_audioStore(
            RESOURCE_ID_DEFAULT,
            MAX_AUDIO, [](Ref<AudioInfo> audio, Ref<AudioInfo> other) -> b8
            { return audio->path == other->path; });

        List<PlayingAudioInfo> s_playingAudios;
    } // namespace

    void AudioInit()
    {
        PROFILE_FUNCTION();
        if (s_isInitialized)
        {
            return;
        }

        s_playingAudios = {};

        INIT_DEVICE();

        // LoadAudio(RelativePath("assets/audios/default.wav"));

        s_isInitialized = TRUE;
    }

    void SetVolume(resource_id_t audio_id, f32 volume)
    {
        PROFILE_FUNCTION();
        if (!s_isInitialized)
        {
            return;
        }

        volume = volume < 0 ? 0 : volume;
        volume = volume > 1 ? 1 : volume;

        auto audioInfo = s_audioStore.Get(audio_id);

        if (audioInfo == nullptr)
        {
            NTT_ENGINE_WARN("The audio id {} is not existed or unloaded", audio_id);
            return;
        }

        SET_VOLUME(audioInfo->sound, volume);
    }

    resource_id_t LoadAudio(const String &path)
    {
        PROFILE_FUNCTION();
        if (!s_isInitialized)
        {
            return RESOURCE_ID_DEFAULT;
        }

        auto existedAudioPaths = s_audioStore.GetByField<String>(
            path,
            [](Ref<AudioInfo> audio) -> String
            { return audio->path; });

        if (existedAudioPaths.size() > 0)
        {
            NTT_ENGINE_WARN("The audio file is already loaded: {}", GetFileName(path, true));
            return RESOURCE_ID_DEFAULT;
        }

        LOAD_SOUND(sound, path);

        if (IS_LOADED_SUCCESS(sound))
        {
            auto info = CreateRef<AudioInfo>(sound, path);
            return s_audioStore.Add(info);
        }
        else
        {
            NTT_ENGINE_WARN("Loading Audio {} Error", GetFileName(path, true));
            return RESOURCE_ID_DEFAULT;
        }
    }

    void PlayAudio(resource_id_t audio_id, const AudioContext &context)
    {
        PROFILE_FUNCTION();
        if (!s_isInitialized)
        {
            return;
        }

        if (audio_id == INVALID_RESOURCE_ID)
        {
            return;
        }

        auto audioInfo = s_audioStore.Get(audio_id);

        if (audioInfo == nullptr)
        {
            NTT_ENGINE_WARN("The audio id {} is not existed or unloaded", audio_id);
            return;
        }

        s_playingAudios.RemoveItem({audio_id, context});
        s_playingAudios.push_back({audio_id, context});
        PLAY_SOUND(audioInfo->sound);
    }

    void StopAudio(resource_id_t audio_id)
    {
        PROFILE_FUNCTION();
        if (!s_isInitialized)
        {
            return;
        }

        auto audioInfo = s_audioStore.Get(audio_id);

        if (audioInfo == nullptr)
        {
            NTT_ENGINE_WARN("The audio id {} is not existed or unloaded", audio_id);
            return;
        }

        if (IS_SOUND_PLAYING(audioInfo->sound))
        {
            STOP_SOUND(audioInfo->sound);
        }

        s_playingAudios.RemoveItem({audio_id});
    }

    void AudioUpdate(f32 delta)
    {
        PROFILE_FUNCTION();
        if (!s_isInitialized)
        {
            return;
        }

        for (auto &playingAudioInfo : s_playingAudios)
        {
            auto id = playingAudioInfo.id;
            auto audioInfo = s_audioStore.Get(id);

            if (audioInfo == nullptr)
            {
                NTT_ENGINE_WARN("The audio {} was unloaded", id);
                s_playingAudios.RemoveItem(playingAudioInfo);
                continue;
            }

            if (!IS_SOUND_PLAYING(audioInfo->sound))
            {
                playingAudioInfo.playedTimes++;

                if (playingAudioInfo.playedTimes < playingAudioInfo.context.desiredPlayedTimes ||
                    playingAudioInfo.context.desiredPlayedTimes == 0)
                {
                    if (s_playingAudios.Contains(playingAudioInfo))
                    {
                        PLAY_SOUND(audioInfo->sound);
                    }
                }
                else
                {
                    EventContext context;
                    context.u32_data[0] = id;

                    if (s_playingAudios.Contains(playingAudioInfo))
                    {
                        s_playingAudios.RemoveItem(playingAudioInfo);
                    }
                    TriggerEvent(NTT_EVENT_AUDIO_FINISHED, nullptr, context);
                }
            }
        }
    }

    void UnloadAudio(resource_id_t audio_id)
    {
        PROFILE_FUNCTION();
        if (!s_isInitialized)
        {
            return;
        }

        auto audioInfo = s_audioStore.Get(audio_id);

        if (audioInfo == nullptr)
        {
            NTT_ENGINE_WARN("The audio id {} is not existed or unloaded", audio_id);
            return;
        }

        StopAudio(audio_id);
        UNLOAD_SOUND(audioInfo->sound);
        s_audioStore.Release(audio_id);
    }

    void AudioShutdown()
    {
        PROFILE_FUNCTION();
        if (!s_isInitialized)
        {
            return;
        }

        auto availableIds = s_audioStore.GetAvailableIds();

        for (auto audioId : availableIds)
        {
            UnloadAudio(audioId);
        }

        s_isInitialized = FALSE;
    }
} // namespace ntt::audio