#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <NTTEngine/application/input_system/internal_input_system.hpp>
#include <NTTEngine/application/input_system/input_system.hpp>
#include <NTTEngine/renderer/GraphicInterface.hpp>
#include "../Fake_GraphicAPI.hpp"

using namespace ntt;
using namespace ntt::input;
using namespace ntt::renderer;

class GraphicInterfaceTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        InputInit(TRUE);
        RendererInit(TRUE);
    }

    void TearDown() override
    {
        RendererShutdown();
        InputShutdown();
    }
};

TEST_F(GraphicInterfaceTest, OnlyDrawWhenGraphicUpdate)
{
    DrawContext context;
    context.color = NTT_WHITE;
    context.fontSize = 12;
    DrawText("Hello World", {0, 0}, context);

    auto texture = LoadTexture("path");
    DrawContext context2;
    context2.priority = 0;
    DrawTexture(texture, {{0, 0}, {0, 0}}, {0, 0}, context2);

    EXPECT_EQ(FakeGraphicAPI::s_instance->m_drawTextCalled, 0);
    EXPECT_EQ(FakeGraphicAPI::s_instance->m_drawTextureCalled, 0);

    GraphicUpdate();

    EXPECT_EQ(FakeGraphicAPI::s_instance->m_drawTextCalled, 1);
    EXPECT_EQ(FakeGraphicAPI::s_instance->m_drawTextureCalled, 1);
}

TEST_F(GraphicInterfaceTest, DrawTextAndTextureWithRightPriority)
{
    DrawContext context;
    context.color = NTT_WHITE;
    context.priority = 1;
    DrawText("Hello World", {0, 0}, context);

    DrawContext context2;
    context2.priority = 0;
    DrawText("Testing", {0, 0}, context2);

    DrawContext context3;
    context3.priority = 0;
    DrawText("Testing 2", {0, 0}, context3);

    auto texture = LoadTexture("path");
    DrawContext context4;
    context4.priority = 1;
    DrawTexture(texture, {{0, 0}, {0, 0}}, {0, 0}, context4);

    auto texture2 = LoadTexture("path");
    DrawContext context5;
    context5.priority = 0;
    context5.tooltip = "TextureComponent 2";
    DrawTexture(texture2, {{0, 0}, {0, 0}}, {0, 0}, context5);

    DrawContext context6;
    context6.priority = 0;
    DrawTexture(34, {{0, 0}, {0, 0}}, {0, 0}, context6); // non-exist texture

    DrawContext context7;
    context7.priority = 255; // out of range priority
    DrawTexture(texture, {{0, 0}, {0, 0}}, {0, 0}, context7);

    GraphicUpdate();

    EXPECT_EQ(FakeGraphicAPI::s_instance->m_drawTextCalled, 3);
    EXPECT_EQ(FakeGraphicAPI::s_instance->m_drawTexts,
              List<String>({"Testing", "Testing 2", "Hello World"}));
    EXPECT_EQ(FakeGraphicAPI::s_instance->m_drawTextureCalled, 2);

    UnloadTexture(texture);
    EXPECT_NO_THROW(UnloadTexture(texture));
}

TEST_F(GraphicInterfaceTest, ValidateSizeTest)
{
    FakeGraphicAPI::s_instance->m_expectedTexture = Texture2D(nullptr, 100, 200);

    auto texture = LoadTexture("path");

    auto size = ValidateSize(texture, {{}, {50, 50}});
    EXPECT_EQ(size, (Size{50, 50}));

    size = ValidateSize(texture, {{}, {200, SIZE_DEFAULT}});
    EXPECT_EQ(size, (Size{200, 400}));

    size = ValidateSize(texture, {{}, {SIZE_DEFAULT, 150}});
    EXPECT_EQ(size, (Size{75, 150}));

    size = ValidateSize(texture, {{}, {SIZE_DEFAULT, SIZE_DEFAULT}});
    EXPECT_EQ(size, (Size{100, 200}));
}

TEST_F(GraphicInterfaceTest, HoveringChecking)
{
    DrawContext context;
    context.priority = 3;
    context.entity_id = 1;

    auto texture = LoadTexture("path");
    DrawTexture(texture, {{0, 0}, {40, 40}}, {0, 0}, context);

    SetMousePosition({20, 20});

    GraphicUpdate();

    auto hovered = GetHoveredTexture();
    EXPECT_EQ(hovered, List<entity_id_t>({1}));
}

TEST_F(GraphicInterfaceTest, HoveringWithMultipleEntity)
{
    auto texture = LoadTexture("path");

    DrawContext context;
    context.priority = 3;
    context.entity_id = 1;
    context.tooltip = "Entity 1";
    DrawTexture(texture, {{0, 0}, {40, 40}}, {0, 0}, context);

    DrawContext context2;
    context2.priority = 2;
    context2.entity_id = 0;
    context2.tooltip = "Entity 0";
    DrawTexture(texture, {{0, 0}, {80, 80}}, {0, 0}, context2);

    DrawContext context3;
    context3.priority = 3;
    context3.entity_id = 8;
    context3.tooltip = "Entity 8";
    DrawTexture(texture, {{0, 0}, {25, 25}}, {0, 0}, context3);

    DrawContext context4;
    context4.priority = 3;
    context4.entity_id = 9;
    context4.tooltip = "Entity 9";
    DrawTexture(texture, {{50, 50}, {20, 20}}, {0, 0}, context4);

    SetMousePosition({10, 10});
    GraphicUpdate();

    auto hovered = GetHoveredTexture();
    EXPECT_EQ(hovered, List<entity_id_t>({0, 1, 8}));

    // TODO: fix later
    EXPECT_EQ(
        FakeGraphicAPI::s_instance->m_drawTexts,
        List<String>({
            "Entity 1",
            "Entity 8",
        }));
}