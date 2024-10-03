#include <NTTEngine/application/script_system/script_store.hpp>
#include <windows.h>
#include <NTTEngine/dev/store.hpp>
#include <NTTEngine/platforms/path.hpp>
#include <NTTEngine/core/formatter.hpp>
#include <functional>
#include <NTTEngine/core/object.hpp>

namespace ntt::script
{
    using namespace dev::store;
#define EMPTY_FUNC_NAME ""

    using CreateFuncType = void *(*)(void *);
    using DeleteFuncType = void (*)(void *);

    namespace
    {
#ifdef _DEBUG
        u32 s_allocateCalled = 0;
#endif

        struct ScriptData : public Object
        {
            String path;
            HMODULE module;
            CreateFuncType createFunc;
            DeleteFuncType deleteFunc;
        };

        struct ScriptObjectData : public Object
        {
            void *object;
            std::function<void(void *)> deleteFunc;
            resource_id_t scriptId;
        };

        Scope<Store<resource_id_t, ScriptData>> s_scripts;
        Scope<Store<script_object_id_t, ScriptObjectData>> s_objects;
        String s_createFunc = EMPTY_FUNC_NAME;
        String s_deleteFunc = EMPTY_FUNC_NAME;

        GetFieldFunc<ScriptData, String> s_GetPath = [](Ref<ScriptData> obj) -> String
        { return obj->path; };

        void CompileFile(const String &path, const String &output)
        {
            if (IsExist(output))
            {
                return;
            }

            auto command = format(
                "g++ -g -o \"{}\" -I \"{}\" -I \"{}\" \"{}\" -L \"{}\" -lNTTEngine -shared",
                output.RawString(),
                "C:/Users/Acer/Games Dev/state-based-game-engine/include",
                "C:/Users/Acer/Games Dev/state-based-game-engine/examples/Flappy Bird",
                path.RawString(),
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

        s_createFunc = createFunc;
        s_deleteFunc = deleteFunc;
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

        if (!IsExist(outputFile))
        {
            return INVALID_SCRIPT_ID;
        }

        // CompileFile(
        //     file,
        //     outputFile);

        Ref<ScriptData> data = CreateRef<ScriptData>();
        try
        {
            data->module = LoadLibraryA(outputFile.RawString().c_str());

            data->createFunc = reinterpret_cast<CreateFuncType>(
                GetProcAddress(data->module, s_createFunc.RawString().c_str()));

            data->deleteFunc = reinterpret_cast<DeleteFuncType>(
                GetProcAddress(data->module, s_deleteFunc.RawString().c_str()));
            NTT_ENGINE_TRACE("The module {} is loaded - output: {}",
                             GetFileName(file), outputFile);

            // void *obj = data->createFunc(nullptr);
            // reinterpret_cast<Script *>(obj)->OnEnter();
            // data->deleteFunc(obj);
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

    resource_id_t ScriptStoreLoad(
        const char *key,
        void *(*createFunc)(void *),
        void (*deleteFunc)(void *))
    {
        auto id = s_scripts->GetIdsByField(String(key), s_GetPath);

        if (id.size() > 0)
        {
            return id[0];
        }

        Ref<ScriptData> data = CreateRef<ScriptData>();
        data->path = key;
        data->module = nullptr;
        data->createFunc = createFunc;
        data->deleteFunc = deleteFunc;

        return s_scripts->Add(data);
    }

    void ScriptStoreUnload(resource_id_t id)
    {
        auto script = s_scripts->Get(id);

        std::function<resource_id_t(Ref<ScriptObjectData>)> func = [id](Ref<ScriptObjectData> obj)
        {
            return obj->scriptId;
        };

        auto objIds = s_objects->GetIdsByField(id, func);

        for (auto objId : objIds)
        {
            ScriptStoreDeleteObject(objId);
        }

        if (script != nullptr)
        {
            if (script->module != nullptr)
            {
                FreeLibrary(script->module);
            }
            NTT_ENGINE_TRACE("The module {} is freed", script->path);
        }

        s_scripts->Release(id);
    }

    script_object_id_t ScriptStoreCreate(resource_id_t id, void *data)
    {
        auto script = s_scripts->Get(id);

        if (script == nullptr)
        {
            return INVALID_OBJECT_ID;
        }

        auto objData = CreateRef<ScriptObjectData>();
        if (script->createFunc == nullptr || script->deleteFunc == nullptr)
        {
            return INVALID_OBJECT_ID;
        }

        objData->object = script->createFunc(data);
#ifdef _DEBUG
        s_allocateCalled++;
#endif
        objData->scriptId = id;

        return s_objects->Add(objData);
    }

    void *ScriptStoreGetObject(script_object_id_t id)
    {
        auto obj = s_objects->Get(id);

        if (obj == nullptr)
        {
            return nullptr;
        }

        return obj->object;
    }

    void ScriptStoreDeleteObject(script_object_id_t id)
    {
        auto obj = s_objects->Get(id);

        if (obj == nullptr)
        {
            return;
        }

        auto script = s_scripts->Get(obj->scriptId);

        if (script != nullptr)
        {
            if (script->deleteFunc != nullptr)
            {
                script->deleteFunc(obj->object);
#ifdef _DEBUG
                s_allocateCalled--;
#endif
            }
        }

        s_objects->Release(id);
    }

    void ScriptStoreShutdown()
    {
        auto objectIds = s_objects->GetAvailableIds();

        for (auto id : objectIds)
        {
            ScriptStoreDeleteObject(id);
        }

        auto scriptIds = s_scripts->GetAvailableIds();

        for (auto id : scriptIds)
        {
            ScriptStoreUnload(id);
        }

        ASSERT_M(s_objects->GetAvailableIds().size() == 0, "The object store is not empty");

        ASSERT_M(s_allocateCalled == 0, "The object is not deleted properly");

        s_objects.reset();
        s_scripts.reset();
    }
};