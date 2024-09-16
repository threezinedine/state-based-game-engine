#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/core/memory.hpp>

namespace ntt::renderer
{
    using namespace ecs;

    class RenderSystem : public System
    {
    public:
        RenderSystem();
        ~RenderSystem();
        // const String GetName() const override { return "Render System"; }

    protected:
        void InitSystemImpl() override;
        void InitEntityImpl(entity_id_t id) override;
        void UpdateImpl(f32 delta, entity_id_t id) override;
        void ShutdownEntityImpl(entity_id_t id) override;
        void ShutdownSystemImpl() override;
        b8 ShouldUpdate(entity_id_t id) override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };

    class SpriteRenderSystem : public System
    {
    public:
        SpriteRenderSystem();
        ~SpriteRenderSystem();
        // const String GetName() const override { return "Sprite Render System"; }

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
} // namespace ntt::renderer
