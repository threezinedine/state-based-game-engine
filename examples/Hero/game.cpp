#include "game.hpp"

using namespace ntt;
using namespace ntt::ecs;
using namespace ntt::renderer;
using namespace ntt::physics;
using namespace ntt::input;
using namespace ntt::log;

static texture_id_t s_birdTexture;
static texture_id_t s_pipeTexture;
static texture_id_t s_base;
static texture_id_t s_gameOver;

static entity_id_t m_gameOver;

struct Pipe : public ComponentBase
{
    Pipe() = default;
};

Game::Game()
    : m_createdPipeCount(0), m_state(GameState::GAME_STATE_IDLE)
{
    s_birdTexture = LoadTexture(RelativePath("images/game-objects/bird.png"), {3, 1});
    s_pipeTexture = LoadTexture(RelativePath("images/game-objects/pipe-green.png"), {1, 1});
    s_base = LoadTexture(RelativePath("images/game-objects/base.png"), {1, 1});
    s_gameOver = LoadTexture(RelativePath("images/ui/gameover.png"), {1, 1});

    ECSRegister(
        "PipeHandler",
        std::bind(&Game::PipeHandling, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
        {typeid(Geometry), typeid(Pipe)});

    auto windowSize = GetWindowSize();

    ECSCreateEntity(
        "Base-up",
        {
            ECS_CREATE_COMPONENT(Geometry,
                                 windowSize.width / 2,
                                 windowSize.height / 16,
                                 windowSize.width,
                                 windowSize.height / 8,
                                 180),
            ECS_CREATE_COMPONENT(Texture, s_base),
        });

    ECSCreateEntity(
        "Base-down",
        {
            ECS_CREATE_COMPONENT(Geometry,
                                 windowSize.width / 2,
                                 windowSize.height / 16 * 15,
                                 windowSize.width,
                                 windowSize.height / 8),
            ECS_CREATE_COMPONENT(Texture, s_base),
        });

    s_gameOver = ECSCreateEntity(
        "Game Over",
        {
            ECS_CREATE_COMPONENT(Geometry, windowSize.width / 2, windowSize.height / 2, 300, 100),
            ECS_CREATE_COMPONENT(Texture, s_gameOver),
        });

    ECSSetComponentActive(s_gameOver, typeid(Geometry), FALSE);

    m_bird = ECSCreateEntity(
        "Bird",
        {
            ECS_CREATE_COMPONENT(Geometry, 200, 200, 70),
            ECS_CREATE_COMPONENT(Texture, s_birdTexture),
            {typeid(Mass), CreateRef<Mass>(1.0f)},
            ECS_CREATE_COMPONENT(Collision),
            ECS_CREATE_COMPONENT(Sprite,
                                 List<std::pair<u8, u8>>{{0, 0}, {1, 0}, {2, 0}},
                                 200),
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
    auto birdGeo = ECS_GET_COMPONENT(m_bird, Geometry);
    auto mass = ECS_GET_COMPONENT(m_bird, Mass);

    if (mass->velocity_y > 0)
    {
        birdGeo->rotatation = 35;
    }
    else if (mass->velocity_y == 0)
    {
        birdGeo->rotatation = 0;
    }
    else
    {
        birdGeo->rotatation = -35;
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
            m_pipeTimer.Reset();

            for (auto i = 0; i < m_pipes.Length(); i++)
            {
                ECSDeleteEntity(m_pipes[i]);
            }

            m_pipes.Clear();
            ECSSetComponentActive(s_gameOver, typeid(Geometry), FALSE);
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

    auto posY = Random(windowSize.height * 5 / 8, windowSize.height * 7 / 8);
    auto halfHeight = windowSize.height - posY;

    auto gap = 150;

    auto downHeight = posY - halfHeight - gap;
    auto posDownY = posY - gap - halfHeight - downHeight / 2;

    auto upPipe = ECSCreateEntity(
        pipeNameUp,
        {
            ECS_CREATE_COMPONENT(Geometry, posX, posY, 100, halfHeight * 2),
            ECS_CREATE_COMPONENT(Texture, s_pipeTexture),
            ECS_CREATE_COMPONENT(Mass, 1.0f, -.1, 0, 0, 0),
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

void Game::PipeHandling(f32 delta, entity_id_t id, List<entity_id_t> others)
{
    auto geo = ECS_GET_COMPONENT(id, Geometry);

    auto mass = ECS_GET_COMPONENT(id, Mass);

    if (geo->x + geo->width / 2 < 0)
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

    ECSSetComponentActive(s_gameOver, typeid(Geometry), TRUE);
    m_start = FALSE;
}

Game::~Game()
{
    ECSDeleteEntity(m_bird);
    UnloadTexture(s_base);
    UnloadTexture(s_pipeTexture);
    UnloadTexture(s_birdTexture);
}