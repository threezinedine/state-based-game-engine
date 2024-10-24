#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/core/parser/json.hpp>
#include <NTTEngine/core/object.hpp>

namespace ntt
{
    using resource_id_t = u32;
    constexpr resource_id_t RESOURCE_ID_DEFAULT = u32{0};

    /**
     * All resource types which are supported by the engine.
     */
    enum ResourceType
    {
        IMAGE = 0x00,  ///< Only .png files are supported.
        AUDIO = 0x01,  ///< Only .wav files are supported.
        SCRIPT = 0x02, ///< Only .cpp files are supported.
        FSM = 0x03,    ///< Only .cpp files are supported.

        TEST = 0xFF, ///< Only for testing purposes.
    };

    /**
     * The struct which stores all needed informations for a
     *    resource.
     */
    struct ResourceInfo
    {
        String name;          ///< The easy to use name of the resource.
        ResourceType type;    ///< The type of the resource.
        String path;          ///< The absolute path to the resource file.
        JSON addintionalInfo; ///< Other needed information for the resource.

        void From(const JSON &data);
        JSON ToJSON() const;

        void OnEditorUpdate(std::function<void()> onChanged = nullptr, void *data = nullptr);
    };

} // namespace ntt
