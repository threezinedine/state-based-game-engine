#include <NTTEngine/renderer/ParentSystem.hpp>
#include <NTTEngine/renderer/Geometry.hpp>
#include <NTTEngine/renderer/Parent.hpp>
#include <cmath>

namespace ntt
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
        geo->pos.x = parentGeo->pos.x + parent->relPos.x * cos(rotation) - parent->relPos.y * sin(rotation);
        geo->pos.y = parentGeo->pos.y + parent->relPos.x * sin(rotation) + parent->relPos.y * cos(rotation);

        geo->rotation = parentGeo->rotation;
    }

    void ParentSystem::ShutdownEntity(entity_id_t id)
    {
    }

    void ParentSystem::ShutdownSystem()
    {
    }
} // namespace ntt
