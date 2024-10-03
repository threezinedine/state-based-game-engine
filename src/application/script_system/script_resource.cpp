#include <NTTEngine/application/script_system/ScriptResource.hpp>
#include <NTTEngine/application/script_system/script_system.hpp>
#include <NTTEngine/platforms/path.hpp>
#include <NTTEngine/core/logging.hpp>

namespace ntt
{
#define THIS(exp) this->m_impl->exp
    using namespace script;
    using namespace log;

    namespace
    {
    } // namespace

    class ScriptResource::Impl
    {
    public:
        const String m_name = "ScriptResource";
        String m_path;
        resource_id_t m_scriptId = INVALID_SCRIPT_ID;
        String m_tempPath;
        String m_outputPath;

        void GetTempPath()
        {
            m_tempPath = JoinPath({
                                      CurrentDirectory(),
                                      "temp",
                                      GetFileName(m_path),
                                  },
                                  FALSE);
        }

        void GetOutputPath()
        {
            String output = GetFileName(m_path);
            output.Replace(".cpp", ".dll");
            m_outputPath = JoinPath({
                                        CurrentDirectory(),
                                        output,
                                    },
                                    FALSE);
        }

        void CreateTempFolder()
        {
            auto tempPath = JoinPath({CurrentDirectory(), "temp"});
            CreateFolder(tempPath);
        }

        void Copy()
        {
            NTT_ENGINE_DEBUG("Copying the file {} to {}", m_path, m_tempPath);
            try
            {
                // std::system(
                //     format("xcopy \"{}\" \"{}\" /Y",
                //            m_path,
                //            GetFileFolder(m_tempPath))
                //         .RawString()
                //         .c_str());
                auto content = ReadFile(m_path);
                OpenFile(m_tempPath);
                Write(content);
                CloseFile();
            }
            catch (const std::exception &e)
            {
                NTT_ENGINE_FATAL(
                    "The file {} cannot be copied with error {}",
                    m_path,
                    e.what());
            }
        }

        b8 FileChanged()
        {
            if (!IsExist(m_tempPath) || !IsExist(m_outputPath))
            {
                return true;
            }

            auto fileContent = ReadFile(m_path);
            auto tempContent = ReadFile(m_tempPath);

            return fileContent != tempContent;
        }

        void CompileFile()
        {
            auto command = format(
                "g++ -g -o \"{}\" -I \"{}\" -I \"{}\" \"{}\" -L \"{}\" -lNTTEngine -shared",
                m_outputPath,
                "C:/Users/Acer/Games Dev/state-based-game-engine/include",
                "C:/Users/Acer/Games Dev/state-based-game-engine/examples/Flappy Bird",
                m_path,
                "C:/Users/Acer/Games Dev/state-based-game-engine/build");

            NTT_ENGINE_DEBUG("The command is {}", command);
            try
            {
                std::system(command.RawString().c_str());
            }
            catch (const std::exception &e)
            {
                NTT_ENGINE_FATAL(
                    "The file {} cannot be compiled with error {}",
                    m_path,
                    e.what());
            }

            Copy();
        }
    };

    ScriptResource::ScriptResource(const ResourceInfo &info)
        : m_impl(CreateScope<Impl>())
    {
        THIS(m_path) = info.path;
        // THIS(CreateTempFolder());
        THIS(GetTempPath());
        THIS(GetOutputPath());
    }

    ScriptResource::~ScriptResource()
    {
    }

    const String &ScriptResource::GetName() const
    {
        return THIS(m_name);
    }

    resource_id_t ScriptResource::LoadImpl()
    {
        if (!IsExist(THIS(m_tempPath)))
        {
            THIS(CompileFile());
        }

        if (THIS(FileChanged()))
        {
            THIS(CompileFile());
        }

        THIS(m_scriptId) = ScriptStoreLoad(THIS(m_path).RawString().c_str(), []() {});
        return THIS(m_scriptId);
    }

    void ScriptResource::UnloadImpl()
    {
        ScriptStoreUnload(THIS(m_scriptId));
    }
} // namespace ntt
