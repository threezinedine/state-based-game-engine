#pragma once
#include "renderer.hpp"

namespace ntt::renderer
{
    class MouseHoveringSystem : public System
    {
    public:
        MouseHoveringSystem();
        ~MouseHoveringSystem();
        // const String GetName() const override { return "Mouse Hovering System"; }

    protected:
        void InitSystemImpl() override;
        void InitEntityImpl(entity_id_t id) override;
        void UpdateImpl(f32 delta, entity_id_t id) override;
        void ShutdownEntityImpl(entity_id_t id) override;
        void ShutdownSystemImpl() override;

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