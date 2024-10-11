#pragma once
#include <NTTEngine/defines.hpp>
#include <NTTEngine/resources/resources.hpp>
#include <NTTEngine/structures/list.hpp>

namespace ntt
{
    void SaveResourceFile(const String &path, List<ResourceInfo> &resources);
} // namespace ntt
