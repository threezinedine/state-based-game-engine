#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/parser/json.hpp>
#include <NTTEngine/core/memory.hpp>
#include <functional>
#include <NTTEngine/structures/string.hpp>

namespace ntt::renderer
{
    using namespace memory;

    /**
     * The needed information about the texture
     */
    struct Grid
    {
        u8 row; ///< The number of rows in the grid of the texture
        u8 col; ///< The number of columns in the grid of the texture

        Grid(u8 row = 1, u8 col = 1) : row(row), col(col)
        {
        }

        JSON ToJSON() const;
        void FromJSON(const JSON &json);

        void SetResourceName(const String &resourceName);
        void EditorUpdate(std::function<void()> onChanged = nullptr);

    private:
        String m_resourceName;
    };

} // namespace ntt::renderer
