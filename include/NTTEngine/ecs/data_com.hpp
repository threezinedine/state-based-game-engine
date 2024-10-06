#pragma once
#include <NTTEngine/core/parser/json.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include "component_base.hpp"

namespace ntt::ecs
{
    using namespace log;

    /**
     * Automatically created when an entity is created, this component
     *      should not be used by the user only the internal system.
     */
    struct DataComponent : public ComponentBase
    {
        JSON data; ///< The data of the entity

        DataComponent() = default;
    };
} // namespace ntt::ecs
