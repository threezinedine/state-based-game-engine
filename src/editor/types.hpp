#pragma once

#include <NTTEngine/defines.hpp>
#include <NTTEngine/core/parser/json.hpp>
#include <NTTEngine/structures/string.hpp>

namespace ntt
{
    struct ProjectInfo
    {
        String name;
        String path;
        i32 width = 800;
        i32 height = 600;
        String title;
    };

    struct EditorConfig
    {
        String lastProjectPath;
    };
} // namespace ntt
