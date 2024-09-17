#pragma once
#include <NTTEngine/defines.hpp>
#include "entity_info.hpp"

namespace ntt::ecs
{
    /**
     * Store all lifetime functionalitiy of a system.
     */
    class System
    {
    public:
        System();
        virtual ~System();

        // virtual const String GetName() const = 0;

        /**
         * The function which is called at the begginning of the game (once)
         *      for initializing the system.
         */
        void InitSystem();

        /**
         * The function which is called for every entity which are registered
         *      (related to this system) in the ECS system.
         */
        void InitEntity(entity_id_t id);

        /**
         * The function which is called for every entity which are registered
         *      (related to this system) in the ECS system.
         */
        void Update(f32 delta, entity_id_t id);

        /**
         * The function which is called for every entity which are registered
         *      (related to this system) in the ECS system.
         */
        void ShutdownEntity(entity_id_t id);

        /**
         * The function which is called at the end of the game (once)
         *      for cleaning up the system.
         */
        void ShutdownSystem();

    protected:
        virtual void InitSystemImpl() {}
        virtual void InitEntityImpl(entity_id_t id) {}
        virtual void UpdateImpl(f32 delta, entity_id_t id) {}
        virtual void ShutdownEntityImpl(entity_id_t id) {}
        virtual void ShutdownSystemImpl() {}

        /**
         * Use for filtering out which entity should be updated or not.
         *      (useful for some cases where the entity can be drawn but
         *      logic is not needed)
         */
        virtual b8 ShouldUpdate(entity_id_t id);

    private:
        class Impl;
        Scope<Impl> m_impl;
    };

} // namespace ntt::ecs
