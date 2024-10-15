#include <NTTEngine/application/script_system/ScriptResource.hpp>
#include <NTTEngine/application/script_system/script_system.hpp>
#include <NTTEngine/platforms/path.hpp>
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/application/hot_reload_module/hot_reload_module.hpp>
#include <NTTEngine/application/event_system/event_system.hpp>

namespace ntt
{
    using namespace script;
    using namespace log;

    namespace
    {
    } // namespace

    class ScriptResource::Impl
    {
    public:
        String path = "";
        String outputPath = "";
        String sourcePath = "";
        String projectPath = "";
        resource_id_t scriptId = INVALID_SCRIPT_ID;
        b8 editorMode = FALSE;

        void SetOutputPath()
        {
            PROFILE_FUNCTION();

            String outputFile = GetFileName(path);
            outputFile.Replace(".cpp", ".dll");

            outputPath = JoinPath(
                {projectPath,
                 outputFile});
        }

        void CompileFile(const String &file)
        {
            PROFILE_FUNCTION();

            auto command = format(
                "g++ -g -o \"{}\" -I \"{}\" -L\"{}\" \"{}\" -lNTTEngine -shared",
                outputPath,
                JoinPath({sourcePath, "include"}),
                CurrentDirectory(),
                file);

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
        : Resource(info), m_impl(CreateScope<Impl>())
    {
        PROFILE_FUNCTION();

        m_impl->path = info.path;
        m_impl->sourcePath = info.addintionalInfo.Get("sourcePath", CurrentDirectory());
        m_impl->projectPath = info.addintionalInfo.Get("projectPath", CurrentDirectory());
        m_impl->editorMode = info.addintionalInfo.Get<b8>("editorMode", FALSE);

        m_impl->SetOutputPath();
    }

    ScriptResource::~ScriptResource()
    {
        PROFILE_FUNCTION();
    }

    resource_id_t ScriptResource::LoadImpl()
    {
        PROFILE_FUNCTION();
        NTT_ENGINE_DEBUG("The script {} is loaded", GetInfo()->name);
        if (!HotReload_RegisterContains(m_impl->path) && m_impl->editorMode)
        {
            HotReloadRegister(
                m_impl->path,
                [this](const String &file)
                {
                    m_impl->CompileFile(file);
                    EventContext context;
                    context.u32_data[0] = m_impl->scriptId;
                    TriggerEvent(NTT_SCRIPT_FILE_CHANGED, nullptr, context);
                });
        }

        m_impl->scriptId = ScriptStoreLoad(
            GetInfo()->name,
            m_impl->outputPath.RawString().c_str(),
            []() {});

        return m_impl->scriptId;
    }

    void ScriptResource::UnloadImpl()
    {
        PROFILE_FUNCTION();

        ScriptStoreUnload(m_impl->scriptId);
    }
} // namespace ntt
