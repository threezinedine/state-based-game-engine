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

#include <NTTEngine/physics/physics_dev.hpp>

#include <NTTEngine/resources/ResourceManager.hpp>

namespace ntt
{
    using namespace memory;
    using namespace log;
    using namespace renderer;
    using namespace audio;
    using namespace input;
    using namespace ecs;
    using namespace physics;

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
        CollisionInit();
        ResourceInit();

        String resourceConfig = ReadFile(RelativePath("configs/resources.json"));
        if (resourceConfig == "")
        {
            resourceConfig = "{}";
        }

        ResourceLoadConfig(JSON(resourceConfig));

        ECSInit();

        ECSRegister(
            COLLISION_NAME,
            CollisionFunc,
            {typeid(Geometry), typeid(Collision)});

        ECSRegister(
            "Mass System",
            MassFunc,
            {typeid(Mass), typeid(Geometry)});

        ECSRegister(
            "Sprite Render System",
            SpriteRenderFunc,
            {typeid(Sprite), typeid(Texture)});

        ECSRegister(
            "Render System",
            RenderFunc,
            {typeid(Geometry), typeid(Texture)});

        s_phrases.Begin();

        NTT_ENGINE_INFO("The application is started.");

        s_timer.Reset();
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

        ECSShutdown();
        ResourceShutdown();
        CollisionShutdown();
        AudioShutdown();
        RendererShutdown();

        CLOSE_WINDOW();
        NTT_ENGINE_INFO("The application is closed.");

        MemoryShutdown();
    }
} // namespace ntt
