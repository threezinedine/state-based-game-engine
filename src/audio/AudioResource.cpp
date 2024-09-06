#include <NTTEngine/audio/AudioResource.hpp>
#include <NTTEngine/audio/audio.hpp>

namespace ntt::audio
{
    class AudioResource::Impl
    {
    public:
        String name;
        String path;
        JSON additionalInfo;

        b8 isLoaded = FALSE;
        resource_id_t audioId = RESOURCE_ID_DEFAULT;
    };

    AudioResource::AudioResource(const ResourceInfo &info)
    {
        m_Impl = CreateScope<Impl>();
        m_Impl->path = info.path;
        m_Impl->additionalInfo = info.addintionalInfo;
    }

    AudioResource::~AudioResource()
    {
    }

    const String &AudioResource::GetName() const
    {
        return m_Impl->name;
    }

    resource_id_t AudioResource::Load()
    {
        if (m_Impl->isLoaded)
        {
            NTT_ENGINE_WARN("The audio resource is already loaded.");
            return RESOURCE_ID_DEFAULT;
        }

        m_Impl->audioId = LoadAudio(m_Impl->path);

        m_Impl->isLoaded = TRUE;

        return m_Impl->audioId;
    }

    void AudioResource::Unload()
    {
        if (!m_Impl->isLoaded)
        {
            NTT_ENGINE_WARN("The audio resource is already unloaded.");
            return;
        }

        UnloadAudio(m_Impl->audioId);

        m_Impl->isLoaded = FALSE;
    }
} // namespace ntt::audio
