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
#include <NTTEngine/application/script_system/native_system.hpp>
#include <NTTEngine/application/state_system/state_system.hpp>
#include <NTTEngine/resources/ResourceManager.hpp>
#include <NTTEngine/application/layer_system/layer_system.hpp>
#include <NTTEngine/debugging/debugging.hpp>
#include <NTTEngine/debugging/components/components.hpp>

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
    using namespace debugging;

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
        DebugInit();

        CREATE_WINDOW(screenWidth, screenHeight, title);

        s_windowSize.width = static_cast<size_t>(screenWidth);
        s_windowSize.height = static_cast<size_t>(screenHeight);

        RendererInit();
        AudioInit();
        ResourceInit();

        String resourceConfig = ReadFile(RelativePath("configs/resources.json"));
        if (resourceConfig == "")
        {
            resourceConfig = "{}";
        }

        ResourceLoadConfig(JSON(resourceConfig));

        ECSInit();
        ECSRegister(
            "Render System",
            CreateRef<RenderSystem>(),
            {typeid(Geometry), typeid(Texture)});

        ECSRegister(
            "Text Render System",
            CreateRef<TextRenderSystem>(),
            {typeid(Geometry), typeid(Text)});

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

        LayerInit();

        /// Setup 3 layers in the predefined order GAME_LAYER -> UI_LAYER -> DEBUG_LAYER
        ///     then now the user's code will not affect the order of the layer
        BeginLayer(GAME_LAYER);

        BeginLayer(UI_LAYER);
        /// Default entities of the UI_LAYER are defined below

        /// Default entities of the UI_LAYER are defined above

        /// Registering the resource for the debug layer
        ResourceInfo resourceInfo;
        resourceInfo.name = "DebugButtons";
        resourceInfo.type = ResourceType::IMAGE;
        resourceInfo.path = JoinPath({GetFileFolder(__FILE__),
                                      "assets/images/buttons.png"},
                                     FALSE);
        resourceInfo.addintionalInfo = JSON(R"({
            "grid": {
                "row": 12,
                "col": 12
            }  
        })");
        RegisterResource("default", resourceInfo);

        BeginLayer(DEBUG_LAYER);
        /// Default entities of the DEBUG_LAYER are defined below
        auto ent1 = ECSCreateEntity(
            "debug-continue",
            {
                ECS_CREATE_COMPONENT(Geometry,
                                     s_windowSize.width / 2 - 31,
                                     s_windowSize.height * 0.05,
                                     30, 30),
                ECS_CREATE_COMPONENT(Texture, GetResourceID("DebugButtons"),
                                     9,
                                     11,
                                     PRIORITY_0,
                                     "Run until next breakpoint"),
                ECS_CREATE_COMPONENT(NativeScriptComponent, CreateRef<ContinueButton>()),
                ECS_CREATE_COMPONENT(Hovering),
            });

        auto ent2 = ECSCreateEntity(
            "debug-next-frame",
            {
                ECS_CREATE_COMPONENT(Geometry,
                                     s_windowSize.width / 2 + 31,
                                     s_windowSize.height * 0.05,
                                     30, 30),
                ECS_CREATE_COMPONENT(Texture, GetResourceID("DebugButtons"),
                                     9,
                                     11,
                                     PRIORITY_0,
                                     "Run next frame"),
                ECS_CREATE_COMPONENT(NativeScriptComponent, CreateRef<NextFrameButton>()),
                ECS_CREATE_COMPONENT(Hovering),
            });

        /// Default entities of the DEBUG_LAYER are defined above

        /// the game layer is activated at the start of the game
        BeginLayer(GAME_LAYER);
        s_phrases.Begin();
        BeginLayer(GAME_LAYER);
        LayerMakeVisible(GAME_LAYER);

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
        LayerUpdate(delta);
        MouseHoveringSystemUpdate(delta);

        BEGIN_DRAWING();

        ClearBackground(BLACK);
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

        LayerShutdown();
        ECSShutdown();
        ResourceShutdown();
        AudioShutdown();
        RendererShutdown();

        CLOSE_WINDOW();
        NTT_ENGINE_INFO("The application is closed.");

        DebugShutdown();
        MemoryShutdown();
    }
} // namespace ntt
