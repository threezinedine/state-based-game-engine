#include "game.hpp"

using namespace ntt;
using namespace ntt::ecs;
using namespace ntt::renderer;
using namespace ntt::physics;
using namespace ntt::input;
using namespace ntt::log;

#define BIRD_ANGLE 35
#define SAFETY_GAP (1 / 16)
#define BACKGROUND_PIECE_WIDTH 300
#define MOVE_SPEED (0.1)
#define BIRD_POS_X 150
#define SCORE_DIGIT_GAP 1

static texture_id_t s_birdTexture;
static texture_id_t s_pipeTexture;
static texture_id_t s_base;
static texture_id_t s_gameOver;
static texture_id_t s_background;
static texture_id_t s_message;
static texture_id_t s_numbers;

static u8 s_backgroundCount = 0;
static u8 s_backgroundRange = 0;

static u16 s_score = 0;

static entity_id_t m_gameOver;
static entity_id_t s_messageEnt;
static entity_id_t s_scoreEnt;

struct Pipe : public ComponentBase
{
    b8 pass = FALSE;

    Pipe() = default;
};

struct Score : public ComponentBase
{
    entity_id_t hunderedNumber;
    entity_id_t tenNumber;
    entity_id_t oneNumber;

    Score(entity_id_t hundered, entity_id_t ten, entity_id_t one)
        : hunderedNumber(hundered), tenNumber(ten), oneNumber(one) {}
};

Game::Game()
    : m_createdPipeCount(0), m_state(GameState::GAME_STATE_IDLE)
{
    s_birdTexture = LoadTexture(RelativePath("images/game-objects/bird.png"), {3, 1});
    s_pipeTexture = LoadTexture(RelativePath("images/game-objects/pipe-green.png"), {1, 1});
    s_base = LoadTexture(RelativePath("images/game-objects/base.png"), {1, 1});
    s_gameOver = LoadTexture(RelativePath("images/ui/gameover.png"), {1, 1});
    s_background = LoadTexture(RelativePath("images/game-objects/background-day.png"), {1, 1});
    s_message = LoadTexture(RelativePath("images/ui/message.png"), {1, 1});
    s_numbers = LoadTexture(RelativePath("images/ui/numbers.png"), {1, 10});

    NTT_ENGINE_DEBUG(
        "Bird Texture: {}\n"
        "Pipe Texture: {}\n"
        "Base Texture: {}\n"
        "Game Over Texture: {}\n"
        "Background Texture: {}",
        s_birdTexture,
        s_pipeTexture,
        s_base,
        s_gameOver,
        s_background);

    ECSRegister(
        "PipeHandler",
        std::bind(&Game::PipeHandling, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
        {typeid(Geometry), typeid(Pipe)});

    ECSRegister(
        "ScoreHandler",
        std::bind(&Game::HandleScore, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
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
            ECS_CREATE_COMPONENT(Texture, s_background),
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
            ECS_CREATE_COMPONENT(Texture, s_base),
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
            ECS_CREATE_COMPONENT(Texture, s_base),
            ECS_CREATE_COMPONENT(Collision),
        });

    s_gameOver = ECSCreateEntity(
        "Game Over",
        {
            ECS_CREATE_COMPONENT(Geometry, windowSize.width / 2, windowSize.height / 2, 300, 100),
            ECS_CREATE_COMPONENT(Texture, s_gameOver),
        });

    m_bird = ECSCreateEntity(
        "Bird",
        {
            ECS_CREATE_COMPONENT(Geometry, BIRD_POS_X, windowSize.height / 2, 70),
            ECS_CREATE_COMPONENT(Texture, s_birdTexture),
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
            ECS_CREATE_COMPONENT(Texture, s_message),
        });

    auto hundered = ECSCreateEntity(
        "Hundered",
        {
            ECS_CREATE_COMPONENT(Geometry, windowSize.width / 2 - 50, 50, 20, 20),
            ECS_CREATE_COMPONENT(Texture, s_numbers),
        });

    auto ten = ECSCreateEntity(
        "Ten",
        {
            ECS_CREATE_COMPONENT(Geometry, windowSize.width / 2, 50, 20, 20),
            ECS_CREATE_COMPONENT(Texture, s_numbers),
        });

    auto one = ECSCreateEntity(
        "One",
        {
            ECS_CREATE_COMPONENT(Geometry, windowSize.width / 2 + 50, 50, 20, 20),
            ECS_CREATE_COMPONENT(Texture, s_numbers),
        });

    s_scoreEnt = ECSCreateEntity(
        "Score",
        {
            ECS_CREATE_COMPONENT(Geometry, 20, 20),
            ECS_CREATE_COMPONENT(Score, hundered, ten, one),
        });

    ECSSetComponentActive(m_bird, typeid(Mass), FALSE);
    ECSSetComponentActive(m_bird, typeid(Sprite), FALSE);

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
    }

    // NTT_APP_DEBUG("Game State: is-started: {}", m_start);
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
        NTT_APP_DEBUG("Space Pressed");
        if (!m_start)
        {
            m_start = TRUE;
            birdGeo->x = 200;
            birdGeo->y = 200;
            mass->velocity_y = 0;
            ECSSetComponentActive(m_bird, typeid(Mass), TRUE);
            ECSSetComponentActive(m_bird, typeid(Sprite), TRUE);

            for (auto i = 0; i < m_backgrounds.Length(); i++)
            {
                ECSSetComponentActive(m_backgrounds[i], typeid(Mass), TRUE);
            }

            m_pipeTimer.Reset();

            for (auto i = 0; i < m_pipes.Length(); i++)
            {
                ECSDeleteEntity(m_pipes[i]);
            }

            if (m_firstTime)
            {
                ECSDeleteEntity(s_messageEnt);
                m_firstTime = FALSE;
            }

            m_pipes.Clear();
            ECSSetComponentActive(s_gameOver, typeid(Geometry), FALSE);
            s_score = 0;
        }
        else
        {
            mass->velocity_y = -0.3;
        }
    }

    if (m_start)
    {
        if (m_pipeTimer.GetMilliseconds() >= 3000)
        {
            auto windowSize = GetWindowSize();
            RandomizePipe(windowSize.width + 10);
            m_pipeTimer.Reset();
        }
    }
}

void Game::RandomizePipe(position_t posX)
{
    auto pipeNameUp = String("Pipe %d up", m_createdPipeCount);
    auto pipeNameDown = String("Pipe %d down", m_createdPipeCount++);
    auto windowSize = GetWindowSize();

    auto posY = Random(windowSize.height * 3 / 4, windowSize.height * 7 / 8);
    auto halfHeight = windowSize.height - posY;

    auto gap = 150;

    auto downHeight = posY - halfHeight - gap;
    auto posDownY = posY - gap - halfHeight - downHeight / 2;

    auto upPipe = ECSCreateEntity(
        pipeNameUp,
        {
            ECS_CREATE_COMPONENT(Geometry, posX, posY, 100, halfHeight * 2),
            ECS_CREATE_COMPONENT(Texture, s_pipeTexture),
            ECS_CREATE_COMPONENT(Mass, 1.0f, -MOVE_SPEED, 0, 0, 0),
            ECS_CREATE_COMPONENT(Collision),
            ECS_CREATE_COMPONENT(Pipe),
        });

    auto downPipe = ECSCreateEntity(
        pipeNameDown,
        {
            ECS_CREATE_COMPONENT(Geometry, posX, posDownY, 100, downHeight, 180),
            ECS_CREATE_COMPONENT(Texture, s_pipeTexture),
            ECS_CREATE_COMPONENT(Mass, 1.0f, -.1, 0, 0, 0),
            ECS_CREATE_COMPONENT(Collision),
            ECS_CREATE_COMPONENT(Pipe),
        });

    m_pipes.Add(upPipe);
    m_pipes.Add(downPipe);
}

void Game::HandleScore(f32 delta, entity_id_t id, List<entity_id_t> others)
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

void Game::PipeHandling(f32 delta, entity_id_t id, List<entity_id_t> others)
{
    auto geo = ECS_GET_COMPONENT(id, Geometry);

    auto mass = ECS_GET_COMPONENT(id, Mass);

    if (geo->x + geo->width / 2 < BIRD_POS_X)
    {
        auto pipe = ECS_GET_COMPONENT(id, Pipe);
        if (!pipe->pass && geo->rotatation == 0)
        {
            pipe->pass = TRUE;
            s_score++;
            NTT_APP_DEBUG("Score: {}", s_score);
        }
    }

    if (geo->x + geo->width < 0)
    {
        ECSDeleteEntity(id);
    }
}

void Game::OnBirdCollide(List<entity_id_t> others)
{
    ECSSetComponentActive(m_bird, typeid(Mass), FALSE);
    ECSSetComponentActive(m_bird, typeid(Sprite), FALSE);

    for (auto i = 0; i < m_pipes.Length(); i++)
    {
        ECSSetComponentActive(m_pipes[i], typeid(Mass), FALSE);
    }

    for (auto i = 0; i < m_backgrounds.Length(); i++)
    {
        ECSSetComponentActive(m_backgrounds[i], typeid(Mass), FALSE);
    }

    ECSSetComponentActive(s_gameOver, typeid(Geometry), TRUE);
    m_start = FALSE;
}

Game::~Game()
{
    ECSDeleteEntity(m_bird);
    UnloadTexture(s_birdTexture);
    UnloadTexture(s_pipeTexture);
    UnloadTexture(s_base);
    UnloadTexture(s_gameOver);
    UnloadTexture(s_background);
}