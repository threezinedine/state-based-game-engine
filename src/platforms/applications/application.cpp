#include <NTTEngine/platforms/application.hpp>
#include "application_platform.hpp"
#include <NTTEngine/core/logging.hpp>
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
#include <NTTEngine/application/state_system/state_system.hpp>
#include <NTTEngine/resources/ResourceManager.hpp>
#include <NTTEngine/application/hot_reload_module/hot_reload_module.hpp>
#include <NTTEngine/application/script_system/script_system.hpp>

namespace ntt
{
    using namespace memory;
    using namespace log;
    using namespace renderer;
    using namespace audio;
    using namespace input;
    using namespace ecs;
    using namespace physics;
    using namespace script;

    namespace
    {
        Phrases s_phrases;
        Timer s_timer;
        Size s_windowSize = {0, 0};

        JSON s_config("{}");
    } // namespace

    void ApplicationInit(u16 screenWidth,
                         u16 screenHeight,
                         const char *title,
                         const Phrases &phrases)
    {
        PROFILE_FUNCTION();
        s_phrases = phrases;

        MemoryInit();
        HotReloadInit();
        ScriptStoreInit("CreateInstance", "DeleteInstance");

        s_windowSize.width = static_cast<size_t>(screenWidth);
        s_windowSize.height = static_cast<size_t>(screenHeight);

        RendererInit();
        ResourceInit();
        InputInit();
        AudioInit();

        String resourceConfig = ReadFile(RelativePath("assets/configs/resources.json"));
        if (resourceConfig == "")
        {
            resourceConfig = "{}";
        }

        ResourceLoadConfig(JSON(resourceConfig));

        CREATE_WINDOW(screenWidth, screenHeight, title);
        ResourceStart();

        ECSInit();
        ECSRegister(
            "Render System",
            CreateRef<RenderSystem>(),
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
            {typeid(Hovering), typeid(Texture), typeid(Geometry)});

        ECSRegister(
            "Mass System",
            CreateRef<MassSystem>(),
            {typeid(Mass), typeid(Geometry)});

        ECSRegister(
            "Sprite Render System",
            CreateRef<SpriteRenderSystem>(),
            {typeid(Sprite), typeid(Texture)});

        /// Setup 3 layers in the predefined order GAME_LAYER -> UI_LAYER -> DEBUG_LAYER
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
        ECSBeginLayer(DEBUG_LAYER);

        ECSBeginLayer(GAME_LAYER);
        s_phrases.Begin();
        ECSBeginLayer(GAME_LAYER);
        ECSLayerMakeVisible(GAME_LAYER);

        NTT_ENGINE_INFO("The application is started.");

        s_timer.Reset();
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
        ECSUpdate(delta);
        MouseHoveringSystemUpdate(delta);

        BEGIN_DRAWING();

        ClearBackground(::BLACK);
        s_phrases.MainLoop(delta);

        GraphicUpdate();
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

    void ApplicationShutdown()
    {
        PROFILE_FUNCTION();
        s_phrases.Close();

        ECSShutdown();

        AudioShutdown();
        InputShutdown();
        ResourceShutdown();
        RendererShutdown();

        CLOSE_WINDOW();
        NTT_ENGINE_INFO("The application is closed.");

        ScriptStoreShutdown();
        HotReloadInit();
        MemoryShutdown();
    }
} // namespace ntt
