#include <NTTEngine/physics/MassSystem.hpp>
#include <NTTEngine/physics/Mass.hpp>
#include <NTTEngine/renderer/Geometry.hpp>

using namespace ntt;
using namespace ntt::ecs;
using namespace ntt::renderer;

namespace ntt::physics
{
    void MassFunc(f32 delta, entity_id_t id, List<entity_id_t> others)
    {
        auto mass = ECS_GET_COMPONENT(id, Mass);
        auto geo = ECS_GET_COMPONENT(id, Geometry);

        mass->velocity_x += mass->acc_x * delta;
        mass->velocity_y += mass->acc_y * delta;

        geo->x += mass->velocity_x * delta;
        geo->y += mass->velocity_y * delta;
    }
} // namespace ntt::physics
