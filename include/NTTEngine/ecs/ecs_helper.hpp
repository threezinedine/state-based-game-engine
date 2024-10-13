#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/structures/string.hpp>
#include <NTTEngine/core/parser/json.hpp>
#include <NTTEngine/core/memory.hpp>
#include "component_base.hpp"
#include <typeindex>

namespace ntt
{
    using namespace memory;
    using namespace ecs;

    /**
     * JSON format which is used during the transformation between JSON and ECS
     *
     * ``` JSON
     *  {
     *      "Geometry": {
     *          "position": {
     *              "x": 100,
     *              "y": 200
     *          },
     *          "size": {
     *              "width": 50,
     *              "height": 50
     *          },
     *          "rotation": 0.0,
     *          "priority": 0,
     *          "color": [255, 255, 255, 255]
     *      },
     *      "TextureComponent": {
     *          "resource_name": "texture_name",
     *          "current_cell": {
     *              "row": 0,
     *              "col": 0
     *          },
     *          "texture_grid": {
     *              "row": 1,
     *              "col": 1
     *          },
     *          "tooltip": "This is the tooltip" // "" if there's no tooltip
     *      }
     *  }
     *
     */

    /**
     * Transform the JSON object with the above format to the list of components.
     * If the JSON object is not valid, then return an empty list.
     * If there is some components with errors then that component will be ignored
     *      and other components will be added to the list (warning will be logged).
     */
    Dictionary<std::type_index, Ref<ComponentBase>> ECS_From(const JSON &json);

    /**
     * Transform the list of components to the JSON object with the above format.
     */
    JSON ECS_ToJSON(const Dictionary<std::type_index, Ref<ComponentBase>> &components);
} // namespace ntt
