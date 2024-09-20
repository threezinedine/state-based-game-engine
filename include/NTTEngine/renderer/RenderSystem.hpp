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

        void InitSystem() override;
        void InitEntity(entity_id_t id) override;
        void Update(f32 delta, entity_id_t id) override;
        void ShutdownEntity(entity_id_t id) override;
        void ShutdownSystem() override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };

    class TextRenderSystem : public System
    {
    public:
        TextRenderSystem();
        ~TextRenderSystem();

        void InitSystem() override;
        void InitEntity(entity_id_t id) override;
        void Update(f32 delta, entity_id_t id) override;
        void ShutdownEntity(entity_id_t id) override;
        void ShutdownSystem() override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };

    class SpriteRenderSystem : public System
    {
    public:
        SpriteRenderSystem();
        ~SpriteRenderSystem();

        void InitSystem() override;
        void InitEntity(entity_id_t id) override;
        void Update(f32 delta, entity_id_t id) override;
        void ShutdownEntity(entity_id_t id) override;
        void ShutdownSystem() override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
} // namespace ntt::renderer
