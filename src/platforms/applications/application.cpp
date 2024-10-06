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
#include <NTTEngine/editor/editor.hpp>

#include <NTTEngine/application/scene_system/scene_system.hpp>
#include "rlImGui.h"
#include "imgui.h"

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

#ifdef _EDITOR
        RenderTexture s_renderTexture;
#endif
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

        s_windowSize.width = static_cast<ntt_size_t>(screenWidth);
        s_windowSize.height = static_cast<ntt_size_t>(screenHeight);

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
            {typeid(Hovering), typeid(TextureComponent), typeid(Geometry)});

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

        ECSBeginLayer(GAME_LAYER);
        s_phrases.Begin();
        ECSBeginLayer(GAME_LAYER);
        ECSLayerMakeVisible(GAME_LAYER);

        NTT_ENGINE_INFO("The application is started.");

        s_timer.Reset();

#ifdef _EDITOR
        EditorInit();
        rlImGuiSetup(true);
        s_renderTexture = LoadRenderTexture(screenWidth, screenHeight);
        RegisterEvent(
            NTT_EDITOR_STOP,
            [](auto id, void *sender, EventContext context)
            {
                SceneReload();
            });
#else
        EditorInit(FALSE);
#endif
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
#ifdef _EDITOR
        BeginTextureMode(s_renderTexture);
#endif
        ClearBackground(::BLACK);
        s_phrases.MainLoop(delta);

        ECSUpdate(delta);
        MouseHoveringSystemUpdate(delta);

        GraphicUpdate();

        // acutally unload all resources of the scene if needed
        // it must be called after drawing
        ResourceUpdate(delta);
#ifdef _EDITOR
        EndTextureMode();
#endif

#ifdef _EDITOR
        rlImGuiBegin();

        ImGui::Begin("Viewport");
        ImVec2 size = ImGui::GetContentRegionAvail();
        ImVec2 viewportSize = {0, 0};

        f32 aspectRatio = static_cast<f32>(s_windowSize.width) / static_cast<f32>(s_windowSize.height);

        if (aspectRatio * size.y > size.x)
        {
            viewportSize.x = size.x;
            viewportSize.y = size.x / aspectRatio;
        }
        else
        {
            viewportSize.x = size.y * aspectRatio;
            viewportSize.y = size.y;
        }

        ImGui::SetCursorPosX(0);
        ImGui::SetCursorPosX(size.x / 2 - viewportSize.x / 2);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (size.y / 2 - viewportSize.y / 2));

        rlImGuiImageRect(
            &s_renderTexture.texture,
            viewportSize.x,
            viewportSize.y,
            {0, 0,
             static_cast<float>(s_renderTexture.texture.width),
             -static_cast<float>(s_renderTexture.texture.height)});
        ImGui::End();

        static b8 show = TRUE;
        if (show)
        {
            ImGui::ShowDemoWindow(&show);
        }

        rlImGuiEnd();
#endif

        END_DRAWING();
        EditorUpdate(delta);

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
        MemoryShutdown();
    }
} // namespace ntt
