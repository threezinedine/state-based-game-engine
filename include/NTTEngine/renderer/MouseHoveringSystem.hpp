#pragma once
#include "renderer.hpp"

namespace ntt::renderer
{
    class MouseHoveringSystem : public System
    {
    public:
        MouseHoveringSystem();
        ~MouseHoveringSystem();

        void InitSystem() override;
        void InitEntity(entity_id_t id) override;
        void Update(f32 delta, entity_id_t id) override;
        void ShutdownEntity(entity_id_t id) override;
        void ShutdownSystem() override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };

    /**
     * Actually handle the mouse hovering system update.
     * Be called in the game loop.
     */
    void MouseHoveringSystemUpdate(f32 delta);
} // namespace ntt::renderer