#include <NTTEngine/application/script_system/ScriptResource.hpp>
#include <NTTEngine/application/script_system/script_system.hpp>
#include <NTTEngine/platforms/path.hpp>
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/application/hot_reload_module/hot_reload_module.hpp>

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
        String m_name = "ScriptResource";
        String m_path = "";
        String m_outputPath = "";
        resource_id_t m_scriptId = INVALID_SCRIPT_ID;

        void SetOutputPath()
        {
            PROFILE_FUNCTION();

            String outputFile = GetFileName(m_path);
            outputFile.Replace(".cpp", ".dll");

            m_outputPath = JoinPath(
                {CurrentDirectory(),
                 outputFile},
                FALSE);
        }

        void CompileFile(const String &file)
        {
            PROFILE_FUNCTION();

            auto command = format(
                "g++ -g -o \"{}\" -I \"{}\" -I \"{}\" \"{}\" -L \"{}\" -lNTTEngine -shared",
                m_outputPath,
                // "C:/Users/Acer/Games Dev/state-based-game-engine/include",
                JoinPath({GetStoredPath(PathType::NTT_ENGINE), "include"}, FALSE),
                // "C:/Users/Acer/Games Dev/state-based-game-engine/examples/Flappy Bird",
                GetStoredPath(PathType::NTT_SOURCE),
                file,
                // "C:/Users/Acer/Games Dev/state-based-game-engine/build");
                GetStoredPath(PathType::NTT_BINARY));

            NTT_ENGINE_DEBUG("The command is {}", command);
            try
            {
                std::system(command.RawString().c_str());
            }
            catch (const std::exception &e)
            {
                NTT_ENGINE_FATAL(
                    "The file {} cannot be compiled with error {}",
                    file,
                    e.what());
            }
        }
    };

    ScriptResource::ScriptResource(const ResourceInfo &info)
        : m_impl(CreateScope<Impl>())
    {
        PROFILE_FUNCTION();

        THIS(m_name) = info.name;
        THIS(m_path) = info.path;
        THIS(SetOutputPath());

        HotReloadRegister(
            THIS(m_path),
            [this](const String &file)
            {
                THIS(CompileFile(file));
            });
    }

    ScriptResource::~ScriptResource()
    {
        PROFILE_FUNCTION();
    }

    const String &ScriptResource::GetName() const
    {
        return THIS(m_name);
    }

    resource_id_t ScriptResource::LoadImpl()
    {
        PROFILE_FUNCTION();

        THIS(m_scriptId) = ScriptStoreLoad(THIS(m_outputPath).RawString().c_str(), []() {});
        return THIS(m_scriptId);
    }

    void ScriptResource::UnloadImpl()
    {
        PROFILE_FUNCTION();

        ScriptStoreUnload(THIS(m_scriptId));
    }
} // namespace ntt
