#pragma once
#include <NTTEngine/defines.hpp>
#include "entity_info.hpp"
#include "NTTEngine/core/object.hpp"

namespace ntt
{
    /**
     * Store all lifetime functionalitiy of a system.
     */
    class System : public Object
    {
    public:
        virtual ~System() = default;

        /**
         * The function which is called at the begginning of the game (once)
         *      for initializing the system.
         */
        virtual void InitSystem() = 0;

        /**
         * The function which is called for every entity which are registered
         *      (related to this system) in the ECS system.
         */
        virtual void InitEntity(entity_id_t id) = 0;

        /**
         * The function which is called for every entity which are registered
         *      (related to this system) in the ECS system.
         */
        virtual void Update(f32 delta, entity_id_t id) = 0;

        /**
         * The function which is called for every entity which are registered
         *      (related to this system) in the ECS system.
         */
        virtual void ShutdownEntity(entity_id_t id) = 0;

        /**
         * The function which is called at the end of the game (once)
         *      for cleaning up the system.
         */
        virtual void ShutdownSystem() = 0;
    };

} // namespace ntt
