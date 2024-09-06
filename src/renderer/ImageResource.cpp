#include <NTTEngine/renderer/ImageResource.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/renderer/GraphicInterface.hpp>

namespace ntt::renderer
{
    using namespace log;

    class ImageResource::Impl
    {
    public:
        String path;
        String name;
        JSON additionalInfo;

        b8 isLoaded = FALSE;
        resource_id_t textureId = RESOURCE_ID_DEFAULT;
    };

    ImageResource::ImageResource(const ResourceInfo &info)
    {
        m_Impl = CreateScope<Impl>();
        m_Impl->path = info.path;
        m_Impl->additionalInfo = info.addintionalInfo;
        m_Impl->name = info.name;
    }

    ImageResource::~ImageResource()
    {
    }

    const String &ImageResource::GetName() const
    {
        return m_Impl->name;
    }

    resource_id_t ImageResource::Load()
    {
        if (m_Impl->isLoaded)
        {
            NTT_ENGINE_WARN("The image resource is already loaded.");
            return RESOURCE_ID_DEFAULT;
        }

        m_Impl->isLoaded = TRUE;

        Grid grid(1, 1);
        auto gridInfo = m_Impl->additionalInfo.Get<JSON>("grid");
        grid.row = gridInfo.Get("row", grid.row);
        grid.col = gridInfo.Get("col", grid.col);

        m_Impl->textureId = LoadTexture(m_Impl->path, grid);
        return m_Impl->textureId;
    }

    void ImageResource::Unload()
    {
        if (!m_Impl->isLoaded)
        {
            NTT_ENGINE_WARN("The image resource is already unloaded.");
            return;
        }

        UnloadTexture(m_Impl->textureId);
        m_Impl->isLoaded = FALSE;
    }
} // namespace ntt::renderer
