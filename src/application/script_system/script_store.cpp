#include <NTTEngine/application/script_system/script_system.hpp>
#include <windows.h>
#include <NTTEngine/dev/store.hpp>
#include <NTTEngine/platforms/path.hpp>
#include <NTTEngine/core/formatter.hpp>

namespace ntt::script
{
    using namespace dev::store;

    namespace
    {
        struct ScriptData
        {
            String path;
            HMODULE module;
        };

        struct ScriptObjectData
        {
            void *object;
            resource_id_t scriptId;
        };

        Scope<Store<resource_id_t, ScriptData>> s_scripts;
        Scope<Store<script_object_id_t, ScriptObjectData>> s_objects;

        GetFieldFunc<ScriptData, String> s_GetPath = [](Ref<ScriptData> obj) -> String
        { return obj->path; };

        void CompileFile(const String &path, const String &output)
        {
            auto command = format(
                "g++ -o \"{}\" \"{}\" -L\"{}\" -lNTTEngine -I\"{}\" -shared",
                output.RawString(),
                path.RawString(),
                "C:/Users/Acer/Games Dev/state-based-game-engine/build",
                "C:/Users/Acer/Games Dev/state-based-game-engine/include");

            NTT_ENGINE_DEBUG("Compile the file {} with command {}", path, command);

            try
            {
                std::system(command.RawString().c_str());
            }
            catch (const std::exception &e)
            {
                NTT_ENGINE_FATAL(
                    "The file {} cannot be compiled with error {}",
                    path,
                    e.what());
            }
        }
    } // namespace

    void ScriptStoreInit(const char *createFunc, const char *deleteFunc)
    {
        s_scripts = CreateScope<Store<resource_id_t, ScriptData>>(
            0, 1000, [](Ref<ScriptData> a, Ref<ScriptData> b)
            { return a->path == b->path; });
        s_objects = CreateScope<Store<script_object_id_t, ScriptObjectData>>(
            0, 1000, [](Ref<ScriptObjectData> a, Ref<ScriptObjectData> b)
            { return FALSE; });
    }

    resource_id_t ScriptStoreLoad(const char *file, std::function<void()> onLoad)
    {
        auto ids = s_scripts->GetIdsByField(String(file), s_GetPath);

        if (ids.size() > 0)
        {
            return ids[0];
        }

        String outputFile = JoinPath({CurrentDirectory(),
                                      GetFileName(file, false)},
                                     FALSE);
        outputFile.Replace(".cpp", ".dll");

        CompileFile(
            file,
            outputFile);

        Ref<ScriptData> data = CreateRef<ScriptData>();
        try
        {
            data->module = LoadLibraryA(outputFile.RawString().c_str());
        }
        catch (const std::exception &e)
        {
            NTT_ENGINE_FATAL("The script {} cannot be loaded with error {}", file, e.what());
            return INVALID_SCRIPT_ID;
        }
        data->path = file;

        auto script_id = s_scripts->Add(data);

        return script_id;
    }

    void ScriptStoreUnload(resource_id_t id)
    {
        auto script = s_scripts->Get(id);

        if (script != nullptr)
        {
            FreeLibrary(script->module);
        }

        s_scripts->Release(id);
    }

    script_object_id_t ScriptStoreCreate(resource_id_t id, void *data)
    {
        return INVALID_OBJECT_ID;
    }

    void *ScriptStoreGetObject(script_object_id_t id)
    {
        return nullptr;
    }

    void ScriptStoreShutdown()
    {
        s_scripts.reset();
        s_objects.reset();
    }
};