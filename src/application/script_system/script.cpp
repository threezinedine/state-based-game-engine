#include <NTTEngine/application/script_system/native_script.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/physics/Collision.hpp>

namespace ntt::script
{
    using namespace ecs;
    using namespace log;
    using namespace memory;
    using namespace physics;

    void Script::OnEnter()
    {
        PROFILE_FUNCTION();

        auto hovering = GetComponent<Hovering>();

        if (hovering != nullptr)
        {
            hovering->callback =
                std::bind(&Script::OnHover, this, std::placeholders::_1);

            hovering->onEnterCallback =
                std::bind(&Script::OnHoverEnter, this);

            hovering->onExitCallback =
                std::bind(&Script::OnHoverExit, this);
        }

        auto collision = GetComponent<Collision>();

        if (collision != nullptr)
        {
            collision->callback =
                std::bind(&Script::OnCollide, this, std::placeholders::_1);
        }

        OnEnterImpl();
    }

    void Script::OnExit()
    {
        PROFILE_FUNCTION();
        Delete();
    }

    void Script::OnUpdate(f32 deltaTime)
    {
        PROFILE_FUNCTION();
        OnUpdateImpl(deltaTime);
    }

}