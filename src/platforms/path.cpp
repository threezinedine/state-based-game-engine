#include <NTTEngine/platforms/path.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/structures/list.hpp>
#include <filesystem>
#include <fstream>
#include <vector>
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
    using namespace memory;
    using namespace log;

    namespace
    {
        String s_sourcePath = ::std::filesystem::current_path()
                                  .make_preferred()
                                  .string();

        Scope<::std::ofstream> s_file = nullptr;
    } // namespace

    void ConfigureSourcePath(const String &path)
    {
        if (!IsExist(path))
        {
            NTT_ENGINE_WARN("The path {} does not exist", path);
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
            NTT_ENGINE_DEBUG("The file {} does not exist", path);
            return "";
        }

        ::std::ifstream file(path.RawString());
        ::std::string content((::std::istreambuf_iterator<char>(file)),
                              ::std::istreambuf_iterator<char>());
        return content;
    }

    void CreateFolder(const String &path)
    {
        if (!IsExist(path))
        {
            ::std::filesystem::path sysPath(path.RawString());
            try
            {
                // ::std::filesystem::create_directories(sysPath);
                std::system(format("mkdir \"{}\"", sysPath.make_preferred().string())
                                .RawString()
                                .c_str());
            }
            catch (const ::std::exception &e)
            {
                NTT_ENGINE_FATAL("The folder {} cannot be created with error {}", path, e.what());
            }
        }
    }

    void OpenFile(const String &path, b8 append)
    {
        if (s_file != nullptr)
        {
            if (s_file->is_open())
            {
                s_file->close();
            }
        }

        s_file = CreateScope<::std::ofstream>(
            path.RawString(),
            append ? ::std::ios::app : ::std::ios::trunc);
    }

    void Write(const String &content)
    {
        if (s_file == nullptr)
        {
            NTT_ENGINE_WARN("No file is opened, nothing will be written");
            return;
        }

        if (!s_file->is_open())
        {
            NTT_ENGINE_TRACE("The file is not opened, nothing will be written");
            return;
        }

        *s_file << content.RawString();
    }

    void CloseFile()
    {
        if (s_file == nullptr)
        {
            NTT_ENGINE_WARN("No file is opened, nothing will be closed");
            return;
        }

        if (s_file->is_open())
        {
            s_file->close();
        }
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

    String GetFileFolder(const String &file)
    {
        auto sysPath = ::std::filesystem ::path(file.RawString())
                           .make_preferred()
                           .parent_path();

        return sysPath.string();
    }

    String CurrentDirectory()
    {
        return ::std::filesystem::current_path()
            .make_preferred()
            .string();
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
                if (index != 0 || useBase)
                {
                    path += "/";
                }
                path += element;
            } });
        return ::std::filesystem::path(path.RawString()).make_preferred().string();
    }
} // namespace ntt
