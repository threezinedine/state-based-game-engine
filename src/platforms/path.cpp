#include <NTTEngine/platforms/path.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/structures/list.hpp>
#include <filesystem>
#include <fstream>
#include <vector>
#include <NTTEngine/core/memory.hpp>

namespace ntt
{
    namespace fs = ::std::filesystem;

    namespace
    {
        String s_sourcePath = fs::current_path()
                                  .make_preferred()
                                  .string();

        Scope<::std::ofstream> s_file = nullptr;

        String s_storedPaths[3];
    } // namespace

    void ConfigureSourcePath(const String &path)
    {
        if (!IsExist(path))
        {
            NTT_ENGINE_WARN("The path {} does not exist", path);
            return;
        }
        s_sourcePath = fs::absolute(path.RawString())
                           .make_preferred()
                           .string();
    }

    const String &GetStoredPath(PathType type)
    {
        if (static_cast<u32>(type) >= sizeof(s_storedPaths) / sizeof(s_storedPaths[0]))
        {
            NTT_ENGINE_WARN("The type {} is not supported", static_cast<u32>(type));
            return s_sourcePath;
        }

        return s_storedPaths[static_cast<u32>(type)];
    }

    void SetSetPath(PathType type, const String &path)
    {
        if (!IsExist(path))
        {
            NTT_ENGINE_WARN("The path {} does not exist", path);
            return;
        }

        if (static_cast<u32>(type) >= sizeof(s_storedPaths) / sizeof(s_storedPaths[0]))
        {
            NTT_ENGINE_WARN("The type {} is not supported", static_cast<u32>(type));
            return;
        }

        s_storedPaths[static_cast<u32>(type)] = fs::absolute(path.RawString())
                                                    .make_preferred()
                                                    .string();
    }

    String RelativePath(const String &path)
    {
        return JoinPath({s_sourcePath, path});
    }

    String SubtractPath(const String &path, const String &base)
    {
        try
        {
            auto sysPath = fs::path(path.RawString()).make_preferred();
            auto sysBase = fs::path(base.RawString()).make_preferred();

            return fs::relative(sysPath, sysBase)
                .make_preferred()
                .string();
        }
        catch (const ::std::exception &e)
        {
            return path;
        }
    }

    b8 IsExist(const String &path)
    {
        fs::path sysPath = fs::path(path.RawString()).make_preferred();
        return fs::exists(sysPath);
    }

    void NTTCopyFile(const String &source, const String &destination)
    {
        if (!IsExist(source))
        {
            NTT_ENGINE_WARN("The source {} does not exist", source, destination);
            return;
        }

        fs::path srcPath = fs::path(source.RawString()).make_preferred();
        fs::path destPath = fs::path(destination.RawString())
                                .make_preferred();

        fs::path destFolder = fs::path(GetFileFolder(destination).RawString())
                                  .make_preferred();

        try
        {
            if (!fs::exists(destFolder))
            {
                fs::create_directories(destFolder);
            }

            if (fs::exists(destPath))
            {
                fs::remove(destPath);
            }
            fs::copy(srcPath, destPath);
        }
        catch (const ::std::exception &e)
        {
            NTT_ENGINE_FATAL("The file {} cannot be copied with error {}", source, e.what());
        }
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
            try
            {
                auto command = format("mkdir \"{}\"", path);
                std::system(command.RawString().c_str());
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
        auto sysPath = fs::path(path.RawString()).make_preferred();
        auto sysPathStr = String(sysPath.string());

        if (containBase)
        {
            if (sysPathStr.Contains(s_sourcePath))
            {
                auto basePath = fs::path(s_sourcePath.RawString()).make_preferred();
                return fs::relative(sysPath, basePath)
                    .make_preferred()
                    .string();
            }
        }

        return sysPath.filename().make_preferred().string();
    }

    String GetFileFolder(const String &file)
    {
        auto sysPath = fs ::path(file.RawString())
                           .make_preferred()
                           .parent_path();

        return sysPath.string();
    }

    String CurrentDirectory()
    {
        return fs::current_path()
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
        return fs::path(path.RawString()).make_preferred().string();
    }

    b8 CheckFileExtension(const String &file, const String &extension)
    {
        auto sysPath = fs::path(file.RawString()).make_preferred();
        return sysPath.extension().string() == extension.RawString();
    }

    String GetFileWithoutExtension(const String &file)
    {
        auto sysPath = fs::path(file.RawString()).make_preferred();
        return sysPath.stem().make_preferred().string();
    }

    List<String> ListFiles(const String &folder, b8 containBase)
    {
        List<String> files;

        if (!IsExist(folder))
        {
            NTT_ENGINE_WARN("The folder {} does not exist", folder);
            return files;
        }

        auto sysFolder = fs::path(folder.RawString()).make_preferred();

        for (const auto &entry : fs::directory_iterator(sysFolder))
        {
            auto path = entry.path();
            if (containBase)
            {
                files.push_back(path.make_preferred().string());
            }
            else
            {
                files.push_back(path.filename().make_preferred().string());
            }
        }

        return files;
    }

    void ClearFolder(const String &folder)
    {
        if (!IsExist(folder))
        {
            NTT_ENGINE_WARN("The folder {} does not exist", folder);
            return;
        }

        auto sysFolder = fs::path(folder.RawString()).make_preferred();

        for (const auto &entry : fs::directory_iterator(sysFolder))
        {
            auto path = entry.path();
            try
            {
                fs::remove_all(path);
            }
            catch (const ::std::exception &e)
            {
                NTT_ENGINE_WARN("The file/folder {} cannot be deleted with error {}",
                                path.string(),
                                e.what());
            }
        }
    }
} // namespace ntt
