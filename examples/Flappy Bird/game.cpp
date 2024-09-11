#include "game.hpp"
#include "PipeController.hpp"
#include "game_state/game_state.hpp"
#include "bird_state/bird_state.hpp"
#include "ScoreBoard.hpp"
#include "BirdController.hpp"

#define SAFETY_GAP (1 / 16)
#define BACKGROUND_PIECE_WIDTH 300
#define BIRD_POS_X 150

#define SPEEDUP_AFTER_DEFAULT 10

static entity_id_t s_scoreEnt;

static JSON s_config = GetConfiguration();

Game::Game()
{
    s_config = GetConfiguration();
    auto windowSize = GetWindowSize();

    ECSCreateEntity(
        {
            ECS_CREATE_COMPONENT(Geometry,
                                 windowSize.width / 2,
                                 windowSize.height / 2,
                                 windowSize.width,
                                 windowSize.height),
            ECS_CREATE_COMPONENT(Texture, GetResourceID("background")),
        });

    ECSCreateEntity(
        {
            ECS_CREATE_COMPONENT(Geometry,
                                 windowSize.width / 2,
                                 windowSize.height / 20,
                                 windowSize.width,
                                 windowSize.height / 10,
                                 180),
            ECS_CREATE_COMPONENT(Texture, GetResourceID("base"), 0, 0, TRUE),
            ECS_CREATE_COMPONENT(Collision),
        });

    ECSCreateEntity(
        {
            ECS_CREATE_COMPONENT(Geometry,
                                 windowSize.width / 2,
                                 windowSize.height / 20 * 19,
                                 windowSize.width,
                                 windowSize.height / 10),
            ECS_CREATE_COMPONENT(Texture, GetResourceID("base"), 0, 0, TRUE),
            ECS_CREATE_COMPONENT(Collision),
        });

    auto birdState = CreateRef<State>();
    birdState->AddChild(BIRD_START, CreateRef<BirdStart>());

    auto gameState = CreateRef<State>();
    gameState->AddChild(START_STATE, CreateRef<GameStart>());
    gameState->AddChild(PLAYING_STATE, CreateRef<GamePlaying>());
    gameState->AddChild(GAME_OVER_STATE, CreateRef<GameOver>());

    ECSCreateEntity(
        {
            ECS_CREATE_COMPONENT(StateComponent, gameState),
        });

    auto birdPlay = CreateRef<BirdPlay>();
    birdPlay->AddChild(BIRD_JUMP, CreateRef<BirdJump>());
    birdPlay->AddChild(BIRD_FALL, CreateRef<BirdFall>());
    birdState->AddChild(BIRD_PLAY, birdPlay);
    birdState->AddChild(BIRD_DEAD, CreateRef<BirdDead>());

    auto bird = ECSCreateEntity(
        {ECS_CREATE_COMPONENT(Geometry, BIRD_POS_X, windowSize.height / 2, 70),
         ECS_CREATE_COMPONENT(Texture, GetResourceID("bird")),
         {typeid(Mass), CreateRef<Mass>(1.0f)},
         ECS_CREATE_COMPONENT(Collision),
         ECS_CREATE_COMPONENT(Sprite,
                              List<std::pair<u8, u8>>{{0, 0}, {1, 0}, {2, 0}},
                              200),
         ECS_CREATE_COMPONENT(NativeScriptComponent, CreateRef<BirdController>()),
         ECS_CREATE_COMPONENT(StateComponent, birdState)});

    ECSCreateEntity(
        {
            ECS_CREATE_COMPONENT(Geometry, 20, 20),
            ECS_CREATE_COMPONENT(NativeScriptComponent, CreateRef<ScoreBoard>()),
        });
}

void Game::Update(f32 delta)
{
}

Game::~Game()
{
}