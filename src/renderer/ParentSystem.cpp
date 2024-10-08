#include <NTTEngine/renderer/ParentSystem.hpp>
#include <NTTEngine/renderer/Geometry.hpp>
#include <NTTEngine/renderer/Parent.hpp>
#include <cmath>

namespace ntt::renderer
{
    class ParentSystem::Impl
    {
    public:
    };

    ParentSystem::ParentSystem()
        : m_impl(CreateScope<Impl>())
    {
    }

    ParentSystem::~ParentSystem()
    {
    }

    void ParentSystem::InitSystem()
    {
    }

    void ParentSystem::InitEntity(entity_id_t id)
    {
        auto geo = ECS_GET_COMPONENT(id, Geometry);
        auto parent = ECS_GET_COMPONENT(id, Parent);

        if (geo == nullptr || parent == nullptr)
        {
            return;
        }

        auto parentGeo = ECS_GET_COMPONENT(parent->parentId, Geometry);

        if (parentGeo == nullptr)
        {
            return;
        }
    }

    void ParentSystem::Update(f32 delta, entity_id_t id)
    {
        auto geo = ECS_GET_COMPONENT(id, Geometry);
        auto parent = ECS_GET_COMPONENT(id, Parent);

        if (geo == nullptr || parent == nullptr)
        {
            return;
        }

        auto parentGeo = ECS_GET_COMPONENT(parent->parentId, Geometry);

        if (parentGeo == nullptr)
        {
            return;
        }

        f32 rotation = parentGeo->rotation * 3.14 / 180;
        geo->x = parentGeo->x + parent->posX * cos(rotation) - parent->posY * sin(rotation);
        geo->y = parentGeo->y + parent->posX * sin(rotation) + parent->posY * cos(rotation);

        geo->rotation = parentGeo->rotation;
    }

    void ParentSystem::ShutdownEntity(entity_id_t id)
    {
    }

    void ParentSystem::ShutdownSystem()
    {
    }
} // namespace ntt::renderer
