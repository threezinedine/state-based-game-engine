#include <NTTEngine/audio/AudioResource.hpp>
#include <NTTEngine/audio/audio.hpp>
#include <NTTEngine/core/profiling.hpp>

namespace ntt::audio
{
    class AudioResource::Impl
    {
    public:
        String name;
        String path;
        JSON additionalInfo;

        b8 isLoaded = FALSE;
        b8 forever = FALSE;
        b8 runAtStart = FALSE;
        f32 volume = 1.0f;
        resource_id_t audioId = RESOURCE_ID_DEFAULT;
    };

    AudioResource::AudioResource(const ResourceInfo &info)
    {
        PROFILE_FUNCTION();
        m_Impl = CreateScope<Impl>();
        m_Impl->path = info.path;
        m_Impl->name = info.name;
        m_Impl->additionalInfo = info.addintionalInfo;

        m_Impl->forever = info.addintionalInfo.Get<b8>("forever", m_Impl->forever);
        m_Impl->volume = info.addintionalInfo.Get<f32>("volume", m_Impl->volume);
        m_Impl->runAtStart = info.addintionalInfo.Get<b8>("runAtStart", m_Impl->runAtStart);
    }

    AudioResource::~AudioResource()
    {
        PROFILE_FUNCTION();
    }

    const String &AudioResource::GetName() const
    {
        PROFILE_FUNCTION();
        return m_Impl->name;
    }

    resource_id_t AudioResource::Load()
    {
        PROFILE_FUNCTION();
        if (m_Impl->isLoaded)
        {
            NTT_ENGINE_WARN("The audio resource is already loaded.");
            return RESOURCE_ID_DEFAULT;
        }

        m_Impl->audioId = LoadAudio(m_Impl->path);

        SetVolume(m_Impl->audioId, m_Impl->volume);

        if (m_Impl->runAtStart)
        {
            AudioContext context;

            if (m_Impl->forever)
            {
                context.desiredPlayedTimes = 0;
            }

            PlayAudio(m_Impl->audioId, context);
        }

        m_Impl->isLoaded = TRUE;

        return m_Impl->audioId;
    }

    void AudioResource::Unload()
    {
        PROFILE_FUNCTION();
        if (!m_Impl->isLoaded)
        {
            NTT_ENGINE_WARN("The audio resource is already unloaded.");
            return;
        }

        UnloadAudio(m_Impl->audioId);

        m_Impl->isLoaded = FALSE;
    }
} // namespace ntt::audio
