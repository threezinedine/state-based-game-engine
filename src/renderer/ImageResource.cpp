#include <NTTEngine/renderer/ImageResource.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/renderer/GraphicInterface.hpp>
#include <NTTEngine/core/profiling.hpp>

namespace ntt::renderer
{
    using namespace log;

    class ImageResource::Impl
    {
    public:
        String path;
        JSON additionalInfo;
        resource_id_t textureId = RESOURCE_ID_DEFAULT;
    };

    ImageResource::ImageResource(const ResourceInfo &info)
        : Resource(info)
    {
        PROFILE_FUNCTION();
        m_Impl = CreateScope<Impl>();
        m_Impl->path = info.path;
        m_Impl->additionalInfo = info.addintionalInfo;
    }

    ImageResource::~ImageResource()
    {
        PROFILE_FUNCTION();
    }

    resource_id_t ImageResource::LoadImpl()
    {
        PROFILE_FUNCTION();
        Grid grid(1, 1);
        auto gridInfo = m_Impl->additionalInfo.Get<JSON>("grid");
        grid.row = gridInfo.Get("row", grid.row);
        grid.col = gridInfo.Get("col", grid.col);

        m_Impl->textureId = LoadTexture(m_Impl->path, grid);

        NTT_ENGINE_DEBUG("Loaded image resource: {}", m_Impl->path);
        return m_Impl->textureId;
    }

    void ImageResource::UnloadImpl()
    {
        PROFILE_FUNCTION();
        UnloadTexture(m_Impl->textureId);
        NTT_ENGINE_DEBUG("Unloaded image resource: {}", m_Impl->path);
    }
} // namespace ntt::renderer
