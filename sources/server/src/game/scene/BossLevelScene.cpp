#include "BossLevelScene.hpp"

#include "AScene.hpp"
#include "Entity.hpp"
#include "EntityUtils.hpp"
#include "Manager.hpp"
#include "PositionComponent.hpp"
#include "Registry.hpp"
#include "SceneManager.hpp"

#include <random>
#include <unistd.h>

using namespace server;

bool bossAlive(Registry &registry)
{
    auto aliveEntityTypes = registry.get_components<EntityTypeComponent>();

    for (const auto &type : aliveEntityTypes)
    {
        if (type.has_value() && type->type == EntityType::BOSS)
        {
            return true;
        }
    }
    return false;
}

void bossAI(Manager &manager, const std::chrono::time_point<std::chrono::high_resolution_clock> &sceneStartTime)
{
    std::vector<Entity> players;
    auto clientMap = manager.getClients();
    static auto lastOrbTime = std::chrono::high_resolution_clock::now();

    for (const auto &client : clientMap)
    {
        if (manager.hasEntityForClient(client.first))
        {
            Entity player = manager.getEntityForClient(client.first);
            players.push_back(player);
        }
    }

    if (players.empty())
        return;

    // Chose a random player to shoot at
    std::random_device random_device;
    std::mt19937 engine {random_device()};
    std::uniform_int_distribution<int> dist(0, players.size() - 1);
    Entity random_player = players[dist(engine)];

    auto optionalPlayerPosition = manager.getRegistry().get_components<PositionComponent>()[random_player];
    if (!optionalPlayerPosition.has_value())
        return;

    PositionComponent playerPosition = optionalPlayerPosition.value();

    VelocityComponent orbVelocity = calculateOrbVelocity(playerPosition, {1300, 500}, 500.0f, 60);

    auto now = std::chrono::high_resolution_clock::now();
    auto timeSinceLastOrb = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastOrbTime).count();

    if (timeSinceLastOrb >= 1000)
    {
        createOrb(manager.getRegistry(), {1400, 520}, orbVelocity);
        // createOrb(manager.getRegistry(), {1250, 490}, orbVelocity);
        // createOrb(manager.getRegistry(), {1250, 550}, orbVelocity);
        lastOrbTime = now;
    }
}

BossLevelScene::BossLevelScene(Manager &manager) : AScene(manager) {}

BossLevelScene::~BossLevelScene() {}

void BossLevelScene::enter()
{
    restartPlayerPositions(*_manager);

    // Add boss
    createBoss(_manager->getRegistry(), {WORLD_MAX_WIDTH - 400, WORLD_MAX_HEIGHT / 2}, {0.0f, 0.0f}, {1000});
}

void BossLevelScene::exit()
{
    // Unload the BossLevelScene scene
    std::cout << "Exiting Third Level" << std::endl;
}

void BossLevelScene::update(const std::chrono::time_point<std::chrono::high_resolution_clock> &sceneStartTime,
                            SceneEvent &event)
{
    // Update the BossLevelScene scene
    processUserInput(*_manager, sceneStartTime);

    _manager->getRegistry().run_systems();

    // Handle boss spawning orbs and shotting them at the player
    bossAI(*_manager, sceneStartTime);

    // If boss is dead, next scene
    if (!bossAlive(_manager->getRegistry()))
    {
        event.event = Event::NEXT;
    }
    StateUpdateMessage state = processOutput(*_manager);
    _manager->pushStateUpdate(state);
}
