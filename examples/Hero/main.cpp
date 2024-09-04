#include <NTTEngine/main.hpp>
#include "component_defs.hpp"
#include "button.hpp"

using namespace ntt;
using namespace ntt::log;
using namespace ntt::input;
using namespace ntt::event;
using namespace ntt::renderer;
using namespace ntt::audio;
using namespace ntt::ecs;

event_id_t id;
texture_id_t button_id;
texture_id_t full_id;
audio_id_t audio_id;
entity_id_t first_entity;
entity_id_t second_entity;

class RenderSystem : public System
{
public:
    void Update(f32 delta, entity_id_t id) override
    {
        auto geometry = ECS_GET_COMPONENT(id, Geometry);
        auto texture = ECS_GET_COMPONENT(id, Texture);
        auto size = DrawTexture(
            texture->id,
            {{geometry->x, geometry->y}, {geometry->width, geometry->height}},
            {texture->rowIndex, texture->colIndex});

        geometry->width = size.width;
        geometry->height = size.height;
    }
};

class HoverCheckingSystem : public System
{
public:
    void Update(f32 delta, entity_id_t id) override
    {
        auto geo = ECS_GET_COMPONENT(id, Geometry);
        auto hovering = ECS_GET_COMPONENT(id, Hovering);
        ntt::Size windowSize = {800, 600};
        auto mousePos = GetMousePosition();

        if (geo->x - geo->width / 2 <= mousePos.x && mousePos.x <= geo->x + geo->width / 2 &&
            geo->y - geo->height / 2 <= mousePos.y && mousePos.y <= geo->y + geo->height / 2)
        {
            if (hovering->isHoverred)
            {
                if (hovering->onEnter != nullptr)
                {
                    hovering->onEnter(delta, id);
                }
            }
            hovering->isHoverred = TRUE;
        }
        else
        {
            if (hovering->isHoverred)
            {
                if (hovering->onBlur != nullptr)
                {
                    hovering->onBlur(delta, id);
                }
            }
            hovering->isHoverred = FALSE;
        }
    }
};

class MoveSytem : public System
{
public:
    void Update(f32 delta, entity_id_t id) override
    {
        auto geometry = ECS_GET_COMPONENT(id, Geometry);
        auto velocity = ECS_GET_COMPONENT(id, Velocity);

        geometry->x += velocity->x * delta;
        geometry->y += velocity->y * delta;
    }
};

class MarginBouncing : public System
{
public:
    void Update(f32 delta, entity_id_t id) override
    {
        auto bouncing = ECS_GET_COMPONENT(id, Bouncing);
        auto geo = ECS_GET_COMPONENT(id, Geometry);

        // TODO: Change later
        Size windowSize{800, 600};

        if (bouncing->isBounce)
        {
            if (geo->x - geo->width / 2 <= 0)
            {
                geo->x = geo->width / 2;
            }
            else if (geo->x + geo->width / 2 >= windowSize.width)
            {
                geo->x = windowSize.width - geo->width / 2;
            }

            if (geo->y - geo->height / 2 <= 0)
            {
                geo->y = geo->height / 2;
            }
            else if (geo->y + geo->height / 2 >= windowSize.height)
            {
                geo->y = windowSize.height - geo->height / 2;
            }
        }
    }
};

void Begin()
{
    NTT_APP_CONFIG(LogLevel::TRACE, LOGGER_CONSOLE, "[@l] - @m");
    id = RegisterEvent(EventCode::KEY_PRESSED,
                       [](EventCode event_code, void *sender, const EventContext &context)
                       { NTT_APP_INFO("Button \"{}\" is pressed",
                                      GetKeyName(static_cast<Key>(context.u16_data[0]))); });

    RegisterEvent(EventCode::AUDIO_FINISHED,
                  [](EventCode event_code, void *sender, const EventContext &context)
                  { NTT_APP_INFO("Audio is finished"); });

    RegisterEvent(EventCode::MOUSE_PRESS,
                  [](EventCode event_code, void *sender, const EventContext &context)
                  { NTT_APP_INFO("Mouse is pressed"); });

    ConfigureSourcePath("C:/Users/Acer/Games Dev/state-based-game-engine/examples/Hero");

    NTT_APP_INFO("Full path: {}",
                 RelativePath("assets/images/button.png"));
    NTT_APP_INFO("File name: {}",
                 GetFileName(RelativePath("assets/images/button.png")));
    NTT_APP_INFO("Relative path: {}",
                 GetFileName(RelativePath("assets/images/button.png"), true));

    button_id = LoadTexture(RelativePath("assets/images/button.png"), {3, 1});

    full_id = LoadTexture(RelativePath("assets/images/new-btn.png"), {2, 1});

    auto new_id = LoadTexture(RelativePath("assets/images/button.png"));
    ASSERT_M(button_id == new_id, "The texture ID is not the same");

    auto non_exist_id = LoadTexture(RelativePath("assets/images/non-exist.png"));
    ASSERT_M(non_exist_id == DEFAULT_TEXTURE, "The id is not default");

    audio_id = LoadAudio(RelativePath("assets/audios/sunflower-street.wav"));
    LoadAudio(RelativePath("assets/audios/sunflower.wav"));

    CreateSystemFunc func = []()
    { return CreateScope<RenderSystem>(); };

    ECSRegister("Bouncing", []()
                { return CreateScope<MarginBouncing>(); },
                {typeid(Geometry), typeid(Bouncing)});
    ECSRegister("Movement", []()
                { return CreateScope<MoveSytem>(); },
                {typeid(Geometry), typeid(Velocity)});
    ECSRegister("Hover Checking", []()
                { return CreateScope<HoverCheckingSystem>(); },
                {typeid(Geometry), typeid(Hovering)});
    ECSRegister("Button System", []()
                { return CreateScope<ButtonSystem>(); },
                {typeid(Geometry),
                 typeid(Texture),
                 typeid(ButtonComponent),
                 typeid(Hovering)});
    ECSRegister("Render", []()
                { return CreateScope<RenderSystem>(); },
                {typeid(Geometry), typeid(Texture)});

    first_entity = ecs::ECSCreateEntity(
        "Hero",
        {
            {typeid(Geometry), CreateRef<Geometry>(0, 100)},
            {typeid(Texture), CreateRef<Texture>(button_id)},
            // {typeid(Bouncing), CreateRef<Bouncing>(TRUE)},
            {typeid(Hovering), CreateRef<Hovering>()},
            {typeid(ButtonComponent), CreateRef<ButtonComponent>()},
        });

    second_entity = ecs::ECSCreateEntity(
        "Test",
        {
            {typeid(Geometry), CreateRef<Geometry>(200, 30, 300)},
            {typeid(Texture), CreateRef<Texture>(full_id)},
        });
}

void DrawingTest();
void ECSTest();
void EventCheck();

void MainLoop(f32 delta)
{
    // DrawingTest();
    if (CheckState(Key::NTT_KEY_UP, InputState::NTT_DOWN))
    {
        auto geo = ECS_GET_COMPONENT(first_entity, Geometry);
        geo->y -= 1 * delta;
    }
    else if (CheckState(Key::NTT_KEY_DOWN, InputState::NTT_DOWN))
    {
        auto geo = ECS_GET_COMPONENT(first_entity, Geometry);
        geo->y += 1 * delta;
    }
    else if (CheckState(Key::NTT_KEY_LEFT, InputState::NTT_DOWN))
    {
        auto geo = ECS_GET_COMPONENT(first_entity, Geometry);
        geo->x -= 1 * delta;
    }
    else if (CheckState(Key::NTT_KEY_RIGHT, InputState::NTT_DOWN))
    {
        auto geo = ECS_GET_COMPONENT(first_entity, Geometry);
        geo->x += 1 * delta;
    }

    ECSTest();
}

void Close()
{
    UnloadAudio(audio_id);
    UnloadTexture(button_id);
    UnloadTexture(full_id);
}

void DrawingTest()
{
    DrawTexture(button_id, {{80, 80}});
    DrawTexture(button_id, {{300, 100}, {100}}, {1, 0});
    DrawTexture(button_id, {{500, 100}, {100, 100}});

    if (CheckState(Key::NTT_KEY_SPACE, InputState::NTT_PRESS))
    {
        PlayAudio(audio_id);
    }
    if (CheckState(Key::NTT_KEY_ENTER, InputState::NTT_PRESS))
    {
        StopAudio(audio_id);
    }
    if (CheckState(Key::NTT_KEY_Z, InputState::NTT_PRESS))
    {
        PlayAudio(DEFAULT_AUDIO);
    }

    if (CheckState(Key::NTT_KEY_X, InputState::NTT_PRESS))
    {
        UnloadAudio(DEFAULT_AUDIO);
    }

    if (CheckState(Key::NTT_KEY_B, InputState::NTT_DOWN))
    {
        DrawTexture(full_id, {{300, 300}, {200}}, {1, 0});
    }
    else
    {
        DrawTexture(full_id, {{300, 300}, {200}}, {0, 0});
    }

    if (CheckState(Key::NTT_KEY_A, InputState::NTT_PRESS))
    {
        UnregisterEvent(id);
    }
}

void ECSTest()
{
}

void EventCheck()
{
    if (CheckState(MouseButton::NTT_BUTTON_LEFT, InputState::NTT_PRESS))
    {
        NTT_ENGINE_DEBUG("Mouse left is pressed");
    }
    if (CheckState(MouseButton::NTT_BUTTON_RIGHT, InputState::NTT_PRESS))
    {
        NTT_ENGINE_DEBUG("Mouse right is pressed");
    }
}