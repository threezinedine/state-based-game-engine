#include <NTTEngine/main.hpp>

using namespace ntt;
using namespace ntt::log;
using namespace ntt::input;
using namespace ntt::event;
using namespace ntt::renderer;

event_id_t id;
texture_id_t test_texture_id;

void Begin()
{
    NTT_APP_CONFIG(LogLevel::TRACE, LOGGER_CONSOLE, "[@l] - @m");
    id = RegisterEvent(EventCode::KEY_PRESSED,
                       [](EventCode event_code, void *sender, const EventContext &context)
                       { NTT_APP_INFO("Button \"%s\" is pressed",
                                      GetKeyName(static_cast<Key>(context.u16_data[0]))); });

    ConfigureSourcePath("C:/Users/Acer/Games Dev/state-based-game-engine");

    NTT_APP_INFO("Full path: {}",
                 RelativePath("assets/images/button.png"));
    NTT_APP_INFO("File name: {}",
                 GetFileName(RelativePath("assets/images/button.png")));
    NTT_APP_INFO("Relative path: {}",
                 GetFileName(RelativePath("assets/images/button.png"), true));

    test_texture_id = LoadTexture("C:/Users/Acer/Games Dev/state-based-game-engine/"
                                  "examples/Hero/assets/images/button.png");

    auto new_id = LoadTexture("C:/Users/Acer/Games Dev/state-based-game-engine/"
                              "examples/Hero/assets/images/button.png");

    ASSERT_M(test_texture_id == new_id, "The texture ID is not the same");
}

void MainLoop(f32 delta)
{
    DrawTexture(test_texture_id);
    DrawTexture(test_texture_id, {{300, 100}, {100}});
    DrawTexture(test_texture_id, {{500, 100}, {100, 100}});

    if (CheckState(Key::NTT_KEY_A, InputState::NTT_PRESS))
    {
        UnregisterEvent(id);
    }
}

void Close()
{
    UnloadTexture(test_texture_id);
}