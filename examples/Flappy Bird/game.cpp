#include "game.hpp"
#include "BirdController.hpp"
#include "PipeController.hpp"

using namespace ntt;
using namespace ntt::ecs;
using namespace ntt::renderer;
using namespace ntt::physics;
using namespace ntt::input;
using namespace ntt::log;
using namespace ntt::audio;

#define SAFETY_GAP (1 / 16)
#define BACKGROUND_PIECE_WIDTH 300
#define BIRD_POS_X 150
#define SCORE_DIGIT_GAP 1

#define SPEEDUP_AFTER_DEFAULT 10

static u16 s_score = 0;

static entity_id_t s_gameOver;
static entity_id_t s_messageEnt;
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

static f32 GetPipeSpeed()
{
    return s_config.Get<f32>("pipe-speed", 0.1) +
           static_cast<u8>(s_score / s_config.Get<u8>("speedup-after", SPEEDUP_AFTER_DEFAULT)) *
               s_config.Get<f32>("pipe-increase", 0.01);
}

Game::Game()
    : m_createdPipeCount(0)
{
    s_config = GetConfiguration();

    ECSRegister(
        "ScoreHandler",
        {std::bind(&Game::HandleScore, this, std::placeholders::_1, std::placeholders::_2)},
        {typeid(Geometry), typeid(Score)});

    auto windowSize = GetWindowSize();

    ECSCreateEntity(
        "background",
        {
            ECS_CREATE_COMPONENT(Geometry,
                                 windowSize.width / 2,
                                 windowSize.height / 2,
                                 windowSize.width,
                                 windowSize.height),
            ECS_CREATE_COMPONENT(Texture, GetResourceID("background")),
        });

    ECSCreateEntity(
        "Base-up",
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
        "Base-down",
        {
            ECS_CREATE_COMPONENT(Geometry,
                                 windowSize.width / 2,
                                 windowSize.height / 20 * 19,
                                 windowSize.width,
                                 windowSize.height / 10),
            ECS_CREATE_COMPONENT(Texture, GetResourceID("base"), 0, 0, TRUE),
            ECS_CREATE_COMPONENT(Collision),
        });

    s_gameOver = ECSCreateEntity(
        "Game Over",
        {
            ECS_CREATE_COMPONENT(Geometry, windowSize.width / 2, windowSize.height / 2, 300, 100),
            ECS_CREATE_COMPONENT(Texture, GetResourceID("gameover")),
        });

    m_bird = ECSCreateEntity(
        "Bird",
        {
            ECS_CREATE_COMPONENT(Geometry, BIRD_POS_X, windowSize.height / 2, 70),
            ECS_CREATE_COMPONENT(Texture, GetResourceID("bird")),
            {typeid(Mass), CreateRef<Mass>(1.0f)},
            ECS_CREATE_COMPONENT(Collision),
            ECS_CREATE_COMPONENT(Sprite,
                                 List<std::pair<u8, u8>>{{0, 0}, {1, 0}, {2, 0}},
                                 200),
            ECS_CREATE_COMPONENT(NativeScriptComponent, CreateRef<BirdController>()),
        });

    s_messageEnt = ECSCreateEntity(
        "Message",
        {
            ECS_CREATE_COMPONENT(Geometry, windowSize.width / 2, windowSize.height / 2, 200),
            ECS_CREATE_COMPONENT(Texture, GetResourceID("message")),
        });

    auto hundered = ECSCreateEntity(
        "Hundered",
        {
            ECS_CREATE_COMPONENT(Geometry, windowSize.width / 2 - 50, 50, 20, 20),
            ECS_CREATE_COMPONENT(Texture, GetResourceID("numbers"), 0, 0, TRUE),
        });

    auto ten = ECSCreateEntity(
        "Ten",
        {
            ECS_CREATE_COMPONENT(Geometry, windowSize.width / 2, 50, 20, 20),
            ECS_CREATE_COMPONENT(Texture, GetResourceID("numbers"), 0, 0, TRUE),
        });

    auto one = ECSCreateEntity(
        "One",
        {
            ECS_CREATE_COMPONENT(Geometry, windowSize.width / 2 + 50, 50, 20, 20),
            ECS_CREATE_COMPONENT(Texture, GetResourceID("numbers"), 0, 0, TRUE),
        });

    s_scoreEnt = ECSCreateEntity(
        "Score",
        {
            ECS_CREATE_COMPONENT(Geometry, 20, 20),
            ECS_CREATE_COMPONENT(Score, hundered, ten, one),
        });

    CollisionRegister(
        m_bird,
        std::bind(&Game::OnBirdCollide, this, std::placeholders::_1));
}

void Game::Update(f32 delta)
{
    if (m_firstTime)
    {
        ECSSetComponentActive(s_gameOver, typeid(Geometry), FALSE);
    }

    if (CheckState(Key::NTT_KEY_SPACE, InputState::NTT_PRESS))
    {
        if (!m_start)
        {
            m_start = TRUE;
            m_pipeTimer.Reset();

            ResetPipe();

            if (m_firstTime)
            {
                ECSDeleteEntity(s_messageEnt);
                m_firstTime = FALSE;
            }

            auto birdMass = ECS_GET_COMPONENT(m_bird, Mass);
            birdMass->velocity_y = -0.3;
            auto birdGeo = ECS_GET_COMPONENT(m_bird, Geometry);
            birdGeo->y = GetConfiguration().Get<position_t>("start-bird-y", 200);
            ECSSetComponentActive(m_bird, typeid(NativeScriptComponent), TRUE);
            ECSSetComponentActive(m_bird, typeid(Mass), TRUE);
            ECSSetComponentActive(m_bird, typeid(Sprite), TRUE);
            ECSSetComponentActive(m_bird, typeid(Geometry), TRUE);

            ECSSetComponentActive(s_gameOver, typeid(Geometry), FALSE);
            s_score = 0;
            auto windowSize = GetWindowSize();
            CreatePipe(
                windowSize.width + 100, s_score);
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

    if (s_score < 100)
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

        hunderedText->colIndex = static_cast<u8>(s_score / 100);
        tenText->colIndex = static_cast<u8>((s_score % 100) / 10);
        oneText->colIndex = s_score % 10;
    }

    if (s_score < 10)
    {
        ECSSetComponentActive(
            ECS_GET_COMPONENT(id, Score)->tenNumber, typeid(Texture), FALSE);

        oneGeo->x = geo->x;
        oneText->colIndex = s_score;
    }
    else
    {
        ECSSetComponentActive(
            ECS_GET_COMPONENT(id, Score)->tenNumber, typeid(Texture), TRUE);
    }

    if (s_score < 100 && s_score >= 10)
    {
        tenGeo->x = geo->x;
        oneGeo->x = geo->x + tenGeo->width + SCORE_DIGIT_GAP;

        oneText->colIndex = s_score % 10;
        tenText->colIndex = static_cast<u8>(s_score / 10);
    }
}

void Game::OnBirdCollide(List<entity_id_t> others)
{
    ECSSetComponentActive(m_bird, typeid(Mass), FALSE);
    ECSSetComponentActive(m_bird, typeid(Sprite), FALSE);

    StopPipe();

    ECSSetComponentActive(s_gameOver, typeid(Geometry), TRUE);
    if (m_start)
    {
        PlayAudio(GetResourceID("die"));
    }

    m_start = FALSE;
}

Game::~Game()
{
    ECSDeleteEntity(m_bird);
}