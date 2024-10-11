#include "utils.hpp"
#include <NTTEngine/core/parser/json.hpp>
#include <NTTEngine/platforms/path.hpp>

namespace ntt
{
    void SaveResourceFile(const String &path, List<ResourceInfo> &resources)
    {
        List<JSON> jsons =
            resources
                .Map<JSON>([](const ResourceInfo &info, ...) -> JSON
                           { return info.ToJSON(); });

        OpenFile(path);
        Write(JSON::FromList(jsons).ToString());
        CloseFile();
    }
} // namespace ntt
