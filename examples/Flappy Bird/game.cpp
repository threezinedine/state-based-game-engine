#include "game.hpp"

using namespace ntt;
using namespace ntt::ecs;
using namespace ntt::renderer;
using namespace ntt::physics;
using namespace ntt::input;
using namespace ntt::log;
using namespace ntt::audio;

#define BIRD_ANGLE 35
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

struct Pipe : public ComponentBase
{
    b8 pass = FALSE;
    b8 hasAfter = FALSE;

    Pipe()
        : pass(FALSE), hasAfter(FALSE) {}
};

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
    : m_createdPipeCount(0), m_state(GameState::GAME_STATE_IDLE)
{
    s_config = GetConfiguration();
    ECSRegister(
        "PipeHandler",
        std::bind(&Game::PipeHandling, this, std::placeholders::_1, std::placeholders::_2),
        {typeid(Geometry), typeid(Pipe)});

    ECSRegister(
        "ScoreHandler",
        std::bind(&Game::HandleScore, this, std::placeholders::_1, std::placeholders::_2),
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

    auto mass = ECS_GET_COMPONENT(m_bird, Mass);
    mass->AddForceConst(0, 0.001);

    CollisionRegister(
        m_bird,
        std::bind(&Game::OnBirdCollide, this, std::placeholders::_1));
}

void Game::Update(f32 delta)
{
    if (m_firstTime)
    {
        ECSSetComponentActive(s_gameOver, typeid(Geometry), FALSE);
        ECSSetComponentActive(m_bird, typeid(Mass), FALSE);
        ECSSetComponentActive(m_bird, typeid(Sprite), FALSE);
        ECSSetComponentActive(m_bird, typeid(Geometry), FALSE);
    }

    auto birdGeo = ECS_GET_COMPONENT(m_bird, Geometry);
    auto mass = ECS_GET_COMPONENT(m_bird, Mass);

    if (mass->velocity_y > 0)
    {
        birdGeo->rotatation = BIRD_ANGLE;
    }
    else if (mass->velocity_y == 0)
    {
        birdGeo->rotatation = 0;
    }
    else
    {
        birdGeo->rotatation = -BIRD_ANGLE;
    }

    if (CheckState(Key::NTT_KEY_SPACE, InputState::NTT_PRESS))
    {
        if (!m_start)
        {
            m_start = TRUE;
            birdGeo->x = 200;
            birdGeo->y = 200;
            mass->velocity_y = 0;
            ECSSetComponentActive(m_bird, typeid(Mass), TRUE);
            ECSSetComponentActive(m_bird, typeid(Sprite), TRUE);
            ECSSetComponentActive(m_bird, typeid(Geometry), TRUE);

            for (auto background : m_backgrounds)
            {
                ECSSetComponentActive(background, typeid(Mass), TRUE);
            }

            m_pipeTimer.Reset();

            for (auto pipe : m_pipes)
            {
                ECSDeleteEntity(pipe);
            }

            if (m_firstTime)
            {
                ECSDeleteEntity(s_messageEnt);
                m_firstTime = FALSE;
            }

            m_pipes.clear();
            ECSSetComponentActive(s_gameOver, typeid(Geometry), FALSE);
            s_score = 0;
            auto windowSize = GetWindowSize();
            RandomizePipe(windowSize.width + 100);
        }
        else
        {
            mass->velocity_y = -0.3;
            PlayAudio(GetResourceID("wing"));
        }
    }
}

void Game::RandomizePipe(position_t posX)
{
    auto pipeNameUp = String("Pipe %d up", m_createdPipeCount);
    auto pipeNameDown = String("Pipe %d down", m_createdPipeCount++);
    auto windowSize = GetWindowSize();

    auto posYStart = windowSize.height * s_config.Get<f32>("pipe-start-range", 3.0f / 4);
    auto posYEnd = windowSize.height * s_config.Get<f32>("pipe-end-range", 7.0f / 8);
    auto posGapY = Random(posYStart, posYEnd);
    auto gap = s_config.Get<f32>("pipe-gap", 100);

    auto highPipeY = (posGapY - gap / 2) / 2;

    auto lowPipeHeight = windowSize.height - posGapY - gap / 2;
    auto lowPipeY = windowSize.height - lowPipeHeight / 2;

    auto upPipe = ECSCreateEntity(
        pipeNameUp,
        {
            ECS_CREATE_COMPONENT(Geometry, posX, highPipeY, 100, highPipeY * 2, 180),
            ECS_CREATE_COMPONENT(Texture, GetResourceID("pipe")),
            ECS_CREATE_COMPONENT(Mass, 1.0f, -GetPipeSpeed(), 0, 0, 0),
            ECS_CREATE_COMPONENT(Collision),
            ECS_CREATE_COMPONENT(Pipe),
        });

    auto downPipe = ECSCreateEntity(
        pipeNameDown,
        {
            ECS_CREATE_COMPONENT(Geometry, posX, lowPipeY, 100, lowPipeHeight),
            ECS_CREATE_COMPONENT(Texture, GetResourceID("pipe")),
            ECS_CREATE_COMPONENT(Mass, 1.0f, -GetPipeSpeed(), 0, 0, 0),
            ECS_CREATE_COMPONENT(Collision),
            ECS_CREATE_COMPONENT(Pipe),
        });

    m_pipes.push_back(upPipe);
    m_pipes.push_back(downPipe);
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

void Game::PipeHandling(f32 delta, entity_id_t id)
{
    auto geo = ECS_GET_COMPONENT(id, Geometry);
    auto mass = ECS_GET_COMPONENT(id, Mass);
    auto pipe = ECS_GET_COMPONENT(id, Pipe);

    auto windowSize = GetWindowSize();

    if (geo->x <= windowSize.width & !pipe->hasAfter)
    {
        if (geo->rotatation == 0)
        {
            pipe->hasAfter = TRUE;
            RandomizePipe(windowSize.width + s_config.Get<f32>("pipe-distance", 300));
        }
    }

    if (geo->x + geo->width / 2 < BIRD_POS_X)
    {
        if (!pipe->pass && geo->rotatation == 0)
        {
            pipe->pass = TRUE;
            s_score++;
            PlayAudio(GetResourceID("point"));

            if (s_score % s_config.Get<u8>("speedup-after", SPEEDUP_AFTER_DEFAULT) == 0)
            {
                for (auto pipeId : m_pipes)
                {
                    auto pipeMass = ECS_GET_COMPONENT(pipeId, Mass);
                    pipeMass->velocity_x = -GetPipeSpeed();
                }
            }
        }
    }

    if (geo->x + geo->width < 0)
    {
        ECSDeleteEntity(id);
        m_pipes.RemoveItem(id);
    }
}

void Game::OnBirdCollide(List<entity_id_t> others)
{
    ECSSetComponentActive(m_bird, typeid(Mass), FALSE);
    ECSSetComponentActive(m_bird, typeid(Sprite), FALSE);

    for (auto pipe : m_pipes)
    {
        ECSSetComponentActive(pipe, typeid(Mass), FALSE);
    }

    for (auto background : m_backgrounds)
    {
        ECSSetComponentActive(background, typeid(Mass), FALSE);
    }

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