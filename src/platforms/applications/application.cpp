#include <NTTEngine/platforms/application.hpp>
#include "application_platform.hpp"
#include <NTTEngine/core/logging/logging.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/audio/audio.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/core/time.hpp>
#include <NTTEngine/core/profiling.hpp>
#include <NTTEngine/application/input_system/input_system.hpp>
#include <NTTEngine/structures/list.hpp>

#include <NTTEngine/platforms/path.hpp>

#include <NTTEngine/renderer/renderer.hpp>
#include <NTTEngine/renderer/RenderSystem.hpp>
#include <NTTEngine/renderer/GraphicInterface.hpp>
#include <NTTEngine/renderer/MouseHoveringSystem.hpp>

#include <NTTEngine/physics/physics_dev.hpp>
#include <NTTEngine/application/script_system/native_script_system.hpp>
#include <NTTEngine/application/script_system/native_script.hpp>
#include <NTTEngine/resources/ResourceManager.hpp>
#include <NTTEngine/application/hot_reload_module/hot_reload_module.hpp>
#include <NTTEngine/application/script_system/script_system.hpp>
#include <NTTEngine/application/script_system/state_system.hpp>
#include <NTTEngine/application/script_system/state_component.hpp>
#include <NTTEngine/editor/editor.hpp>
#include <NTTEngine/editor/editor_system.hpp>
#include <NTTEngine/renderer/ParentSystem.hpp>

#include <NTTEngine/application/scene_system/scene_system.hpp>

namespace ntt
{
    namespace
    {
        Phrases s_phrases;
        Timer s_timer;
        Size s_windowSize = {0, 0};

        JSON s_config("{}");
        b8 s_editor = FALSE;

        void ApplicationReload()
        {
            ECSRemoveAllEntities();
            // ResourceStart();
            ECSBeginLayer(GAME_LAYER);
            s_phrases.Begin();
            ECSBeginLayer(GAME_LAYER);
            ECSLayerMakeVisible(GAME_LAYER);
        }
    } // namespace

    void ApplicationInit(u16 screenWidth,
                         u16 screenHeight,
                         const char *title,
                         const Phrases &phrases,
                         List<String> sceneNames,
                         b8 editor)
    {
        PROFILE_FUNCTION();
        s_phrases = phrases;
        s_editor = editor;

        auto handlers = List<Ref<Handler>>{
            CreateRef<ConsoleHandler>(),
        };

        if (editor)
        {
            handlers.push_back(CreateRef<EditorLogHandler>());
        }

        NTT_ENGINE_CONFIG(LogLevel::DEBUG, handlers);

        HotReloadInit();
        ScriptStoreInit("CreateInstance", "DeleteInstance", "GetBaseType");

        s_windowSize.width = static_cast<ntt_size_t>(screenWidth);
        s_windowSize.height = static_cast<ntt_size_t>(screenHeight);

        RendererInit();
        ResourceInit(editor);
        InputInit(FALSE, editor);

        // TODO: Refactor this
        SetTraceLogLevel(LOG_NONE);
        AudioInit();

        String resourceConfig = ReadFile(RelativePath("assets/configs/resources.json"));
        if (resourceConfig == "")
        {
            resourceConfig = "{}";
        }

        // ResourceLoadConfig(JSON(resourceConfig));

        // // TODO: Refactor this
        // if (s_editor)
        // {
        //     SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        // }

        // CREATE_WINDOW(screenWidth, screenHeight, title);

        // if (s_editor)
        // {
        //     MaximizeWindow();
        // }

        // ResourceStart();

        ECSInit();

        // Parent system must be front of the render system for
        //      child entity's position to be updated before rendering
        ECSRegister(
            "Parent System",
            CreateRef<ParentSystem>(),
            {typeid(Parent)},
            TRUE);

        ECSRegister(
            "Render System",
            CreateRef<RenderSystem>(),
            {typeid(Geometry)},
            TRUE);

        ECSRegister(
            "Editor System",
            CreateRef<EditorSystem>(),
            {typeid(Geometry)},
            TRUE);

        ECSRegister(
            "Native Script System",
            CreateRef<ScriptSystem>(),
            {typeid(NativeScriptComponent)});

        ECSRegister(
            "State System",
            CreateRef<StateSystem>(),
            {typeid(StateComponent)});

        ECSRegister(
            COLLISION_NAME,
            CreateRef<CollisionSystem>(),
            {typeid(Geometry), typeid(Collision)});

        ECSRegister(
            "Hovering System",
            CreateRef<MouseHoveringSystem>(),
            {typeid(Hovering), typeid(Geometry)});

        ECSRegister(
            "Mass System",
            CreateRef<MassSystem>(),
            {typeid(Mass), typeid(Geometry)});

        ECSRegister(
            "Sprite Render System",
            CreateRef<SpriteRenderSystem>(),
            {typeid(Sprite), typeid(TextureComponent)});

        /// Setup 3 layers in the predefined order GAME_LAYER -> UI_LAYER -> EDITOR_LAYER
        ///     then now the user's code will not affect the order of the layer
        ECSBeginLayer(GAME_LAYER);

        ECSBeginLayer(UI_LAYER_0);
        ECSBeginLayer(UI_LAYER_1);
        ECSBeginLayer(UI_LAYER_2);
        ECSBeginLayer(UI_LAYER_3);
        ECSBeginLayer(UI_LAYER_4);
        ECSBeginLayer(UI_LAYER_5);
        ECSBeginLayer(UI_LAYER_6);
        ECSBeginLayer(UI_LAYER_7);
        ECSBeginLayer(UI_LAYER_8);
        ECSBeginLayer(EDITOR_LAYER);

        NTT_ENGINE_INFO("The application is started.");

        s_timer.Reset();

        // TODO: Refactor this
        if (s_editor)
        {
            SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        }

        CREATE_WINDOW(screenWidth, screenHeight, title);

        if (s_editor)
        {
            MaximizeWindow();
        }

        EditorInit("");

        // ResourceStart();
        ECSBeginLayer(GAME_LAYER);
        s_phrases.Begin();
        ECSBeginLayer(GAME_LAYER);
        ECSLayerMakeVisible(GAME_LAYER);

        if (editor)
        {
            // TriggerEvent(NTT_EDITOR_STOP, {});
        }

        RegisterEvent(
            NTT_APPLICATION_RESET,
            [](auto id, void *sender, EventContext context)
            {
                ApplicationReload();
            });
    }

    void LoadConfiguration(const String &path)
    {
        PROFILE_FUNCTION();
        auto data = ReadFile(path);
        try
        {
            s_config = JSON(data);
        }
        catch (...)
        {
            NTT_ENGINE_WARN("The configuration file is not found or not a JSON file.");
            s_config = JSON("{}");
        }
    }

    JSON &GetConfiguration()
    {
        return s_config;
    }

    void ApplicationUpdate(b8 &running)
    {
        PROFILE_FUNCTION();
        auto delta = static_cast<f32>(s_timer.GetMilliseconds());
        s_timer.Reset();

        InputUpdate(delta);
        AudioUpdate(delta);

        BEGIN_DRAWING();
        EditorBeginDraw();

        ClearBackground(::BLACK);
        s_phrases.MainLoop(delta);

        ECSUpdate(delta);
        MouseHoveringSystemUpdate(delta);

        GraphicUpdate();

        // acutally unload all resources of the scene if needed
        // it must be called after drawing
        // ResourceUpdate(delta);
        EditorEndDraw();

        EditorUpdate(delta);
        END_DRAWING();

        running = !WINDOW_SHOULD_CLOSE();

        EventContext context;
        context.f32_data[0] = 1000.0f / delta; ///< FPS of the game
        TriggerEvent(NTT_END_FRAME, nullptr, context);
    }

    Size &GetWindowSize()
    {
        return s_windowSize;
    }

    void SetWindowSize(const Size &size)
    {
        s_windowSize = size;
    }

    void ApplicationShutdown()
    {
        PROFILE_FUNCTION();
        s_phrases.Close();
        EditorShutdown();
        ECSShutdown();

        AudioShutdown();
        InputShutdown();
        ResourceShutdown();
        RendererShutdown();

        CLOSE_WINDOW();
        NTT_ENGINE_INFO("The application is closed.");

        ScriptStoreShutdown();
        HotReloadInit();
    }
} // namespace ntt
