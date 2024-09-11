#include "game.hpp"
#include "PipeController.hpp"
#include "game_state/game_state.hpp"
#include "bird_state/bird_state.hpp"

#define SAFETY_GAP (1 / 16)
#define BACKGROUND_PIECE_WIDTH 300
#define BIRD_POS_X 150
#define SCORE_DIGIT_GAP 1

#define SPEEDUP_AFTER_DEFAULT 10

static entity_id_t s_scoreEnt;

static JSON s_config = GetConfiguration();

struct Score : public ComponentBase
{
    entity_id_t hunderedNumber;
    entity_id_t tenNumber;
    entity_id_t oneNumber;

    Score(entity_id_t hundered, entity_id_t ten, entity_id_t one)
        : hunderedNumber(hundered), tenNumber(ten), oneNumber(one) {}
};

Game::Game()
{
    s_config = GetConfiguration();
    ECSRegister(
        "ScoreHandler",
        {std::bind(&Game::HandleScore, this, std::placeholders::_1, std::placeholders::_2)},
        {typeid(Geometry), typeid(Score)});

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
         // ECS_CREATE_COMPONENT(NativeScriptComponent, CreateRef<BirdController>()),
         ECS_CREATE_COMPONENT(StateComponent, birdState)});

    auto hundered = ECSCreateEntity(
        {
            ECS_CREATE_COMPONENT(Geometry, windowSize.width / 2 - 50, 50, 20, 20),
            ECS_CREATE_COMPONENT(Texture, GetResourceID("numbers"), 0, 0, TRUE),
        });

    auto ten = ECSCreateEntity(
        {
            ECS_CREATE_COMPONENT(Geometry, windowSize.width / 2, 50, 20, 20),
            ECS_CREATE_COMPONENT(Texture, GetResourceID("numbers"), 0, 0, TRUE),
        });

    auto one = ECSCreateEntity(
        {
            ECS_CREATE_COMPONENT(Geometry, windowSize.width / 2 + 50, 50, 20, 20),
            ECS_CREATE_COMPONENT(Texture, GetResourceID("numbers"), 0, 0, TRUE),
        });

    s_scoreEnt = ECSCreateEntity(
        {
            ECS_CREATE_COMPONENT(Geometry, 20, 20),
            ECS_CREATE_COMPONENT(Score, hundered, ten, one),
        });

    auto gameState = CreateRef<State>();
    gameState->AddChild(START_STATE, CreateRef<GameStart>());
    gameState->AddChild(PLAYING_STATE, CreateRef<GamePlaying>());
    gameState->AddChild(GAME_OVER_STATE, CreateRef<GameOver>());

    ECSCreateEntity(
        {
            ECS_CREATE_COMPONENT(StateComponent, gameState),
        });

    CollisionRegister(
        bird,
        std::bind(&Game::OnBirdCollide, this, std::placeholders::_1));
}

void Game::Update(f32 delta)
{
    if (CheckState(Key::NTT_KEY_SPACE, InputState::NTT_PRESS))
    {
        if (!m_start)
        {
            m_start = TRUE;

            ResetPipe();

            GetGameData()->state = PLAYING;

            auto windowSize = GetWindowSize();
            GetGameData()->score = 0;
            CreatePipe(windowSize.width + 100);
        }
    }
}

void Game::HandleScore(f32 delta, entity_id_t id)
{
    auto geo = ECS_GET_COMPONENT(id, Geometry);

    auto hundredId = ECS_GET_COMPONENT(id, Score)->hunderedNumber;
    auto tenId = ECS_GET_COMPONENT(id, Score)->tenNumber;
    auto oneId = ECS_GET_COMPONENT(id, Score)->oneNumber;

    auto hundredGeo = ECS_GET_COMPONENT(hundredId, Geometry);
    auto tenGeo = ECS_GET_COMPONENT(tenId, Geometry);
    auto oneGeo = ECS_GET_COMPONENT(oneId, Geometry);

    auto hunderedText = ECS_GET_COMPONENT(hundredId, Texture);
    auto tenText = ECS_GET_COMPONENT(tenId, Texture);
    auto oneText = ECS_GET_COMPONENT(oneId, Texture);

    hundredGeo->y = geo->y;
    tenGeo->y = geo->y;
    oneGeo->y = geo->y;

    auto score = GetGameData()->score;

    if (score < 100)
    {
        ECSSetComponentActive(
            ECS_GET_COMPONENT(id, Score)->hunderedNumber, typeid(Texture), FALSE);
    }
    else
    {
        ECSSetComponentActive(
            ECS_GET_COMPONENT(id, Score)->hunderedNumber, typeid(Texture), TRUE);

        hundredGeo->x = geo->x;
        tenGeo->x = geo->x + hundredGeo->width + SCORE_DIGIT_GAP;
        oneGeo->x = geo->x + hundredGeo->width + tenGeo->width + SCORE_DIGIT_GAP * 2;

        hunderedText->colIndex = static_cast<u8>(score / 100);
        tenText->colIndex = static_cast<u8>((score % 100) / 10);
        oneText->colIndex = score % 10;
    }

    if (score < 10)
    {
        ECSSetComponentActive(
            ECS_GET_COMPONENT(id, Score)->tenNumber, typeid(Texture), FALSE);

        oneGeo->x = geo->x;
        oneText->colIndex = score;
    }
    else
    {
        ECSSetComponentActive(
            ECS_GET_COMPONENT(id, Score)->tenNumber, typeid(Texture), TRUE);
    }

    if (score < 100 && score >= 10)
    {
        tenGeo->x = geo->x;
        oneGeo->x = geo->x + tenGeo->width + SCORE_DIGIT_GAP;

        oneText->colIndex = score % 10;
        tenText->colIndex = static_cast<u8>(score / 10);
    }
}

void Game::OnBirdCollide(List<entity_id_t> others)
{
    StopPipe();

    if (m_start)
    {
        PlayAudio(GetResourceID("die"));
    }

    m_start = FALSE;
    GetGameData()->state = GAME_OVER;
}

Game::~Game()
{
}