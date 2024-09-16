#include <NTTEngine/platforms/application.hpp>
#include "application_platform.hpp"
#include <NTTEngine/core/logging.hpp>
#include <NTTEngine/core/memory.hpp>
#include <NTTEngine/audio/audio.hpp>
#include <NTTEngine/ecs/ecs.hpp>
#include <NTTEngine/core/time.hpp>
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
        s_phrases = phrases;

        MemoryInit();

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

        s_phrases.Begin();

        NTT_ENGINE_INFO("The application is started.");

        s_timer.Reset();
        BeginLayer(LayerType::GAME_LAYER);
    }

    void LoadConfiguration(const String &path)
    {
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
        auto delta = static_cast<f32>(s_timer.GetMilliseconds());
        s_timer.Reset();

        InputUpdate(delta);

        BEGIN_DRAWING();

        ClearBackground(BLACK);
        AudioUpdate(delta);
        ECSUpdate(delta);

        s_phrases.MainLoop(delta);

        GraphicUpdate();
        MouseHoveringSystemUpdate(delta);
        LayerUpdate(delta);
        END_DRAWING();

        running = !WINDOW_SHOULD_CLOSE();

        NTT_ENGINE_TRACE("FPS: {0}", 1000.0f / delta);
    }

    Size &GetWindowSize()
    {
        return s_windowSize;
    }

    void ApplicationShutdown()
    {
        s_phrases.Close();

        LayerShutdown();
        ECSShutdown();
        ResourceShutdown();
        AudioShutdown();
        RendererShutdown();

        CLOSE_WINDOW();
        NTT_ENGINE_INFO("The application is closed.");

        MemoryShutdown();
    }
} // namespace ntt
