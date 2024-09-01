#include <NTTEngine/platforms/path.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/structures/list.hpp>
#include <filesystem>
#include <fstream>
#include <vector>

namespace ntt
{
    namespace
    {
        String s_sourcePath = ::std::filesystem::current_path()
                                  .make_preferred()
                                  .string();
    } // namespace

    void ConfigureSourcePath(const String &path)
    {
        if (!IsExist(path))
        {
            NTT_ENGINE_WARN("The path {} does not exist", path.RawString().c_str());
            return;
        }
        s_sourcePath = ::std::filesystem::absolute(path.RawString())
                           .make_preferred()
                           .string();
    }

    String RelativePath(const String &path)
    {
        return JoinPath({s_sourcePath, path}, false);
    }

    b8 IsExist(const String &path)
    {
        return ::std::filesystem::exists(path.RawString());
    }

    String ReadFile(const String &path)
    {
        if (!IsExist(path))
        {
            NTT_ENGINE_WARN("The file {} does not exist", path);
            return "";
        }

        ::std::ifstream file(path.RawString());
        ::std::string content((::std::istreambuf_iterator<char>(file)),
                              ::std::istreambuf_iterator<char>());
        return content;
    }

    String WriteFile(const String &path, const String &content, b8 append)
    {
        ::std::ofstream file(path.RawString(), append ? ::std::ios::app : ::std::ios::trunc);
        file << content.RawString();
        return path;
    }

    String GetFileName(const String &path, b8 containBase)
    {
        auto sysPath = ::std::filesystem::path(path.RawString()).make_preferred();
        auto sysPathStr = String(sysPath.string());

        if (containBase)
        {
            if (sysPathStr.Contains(s_sourcePath))
            {
                auto basePath = ::std::filesystem::path(s_sourcePath.RawString()).make_preferred();
                return ::std::filesystem::relative(sysPath, basePath)
                    .make_preferred()
                    .string();
            }
        }

        return sysPath.filename().make_preferred().string();
    }

    String JoinPath(List<String> paths, b8 useBase)
    {
        String path = useBase ? s_sourcePath : "";
        b8 isFinished = false;

        // TODO: Introduce List iteration later

        paths.ForEach([&](const ntt::String &element, u32 index) -> void
                      {
            if (!isFinished)
            {
                if (index != 0)
                {
                    path += "/";
                }
                path += element;
            } });
        return ::std::filesystem::path(path.RawString()).make_preferred().string();
    }
} // namespace ntt
