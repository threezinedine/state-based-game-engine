#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
    using namespace memory;
    using namespace ecs;

    class EditorSystem : public System
    {
    public:
        EditorSystem();
        ~EditorSystem();

    protected:
        void InitSystem() override;
        void InitEntity(entity_id_t entityId) override;
        void Update(f32 delta, entity_id_t entityId) override;
        void ShutdownEntity(entity_id_t entityId) override;
        void ShutdownSystem() override;

    private:
        class Impl;
        Scope<Impl> m_impl;
    };
} // namespace ntt
