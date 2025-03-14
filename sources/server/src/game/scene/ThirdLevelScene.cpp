#include "ThirdLevelScene.hpp"

#include "Entity.hpp"
#include "EntityUtils.hpp"
#include "PositionComponent.hpp"
#include "SceneManager.hpp"

#include <random>
#include <unistd.h>
#include <vector>

using namespace server;

void thirdLvlMobsAI(Manager &manager, const std::chrono::time_point<std::chrono::high_resolution_clock> &sceneStartTime,
                    bool &firstMobAlive, bool &secondMobAlive)
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

    PositionComponent orbSpawnPoint0 = {1550.0f, 240.0f};
    PositionComponent orbSpawnPoint1 = {1550.0f, 840.0f};

    VelocityComponent orbVelocity0 = calculateOrbVelocity(playerPosition, orbSpawnPoint0, 500.0f, 60);
    VelocityComponent orbVelocity1 = calculateOrbVelocity(playerPosition, orbSpawnPoint1, 500.0f, 60);

    auto now = std::chrono::high_resolution_clock::now();
    auto timeSinceLastOrb = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastOrbTime).count();

    if (timeSinceLastOrb >= 3000)
    {
        if (firstMobAlive)
            createOrb(manager.getRegistry(), orbSpawnPoint0, orbVelocity0);
        if (secondMobAlive)
            createOrb(manager.getRegistry(), orbSpawnPoint1, orbVelocity1);
        lastOrbTime = now;
    }
}

ThirdLevelScene::ThirdLevelScene(Manager &manager) : AScene(manager), _mobs(std::vector<Entity>()) {}

ThirdLevelScene::~ThirdLevelScene() {}

void ThirdLevelScene::enter()
{
    restartPlayerPositions(*_manager);

    // Add entities
    _mobs.push_back(createMob(_manager->getRegistry(), {1500.0f, 250.0f}, {0.0f, 0.0f}, {200}));
    _mobs.push_back(createMob(_manager->getRegistry(), {1500.0f, 850.0f}, {0.0f, 0.0f}, {200}));
}

void ThirdLevelScene::exit()
{
    // Unload the ThirdLevelScene scene
    std::cout << "Exiting Second Level" << std::endl;
}

void ThirdLevelScene::update(const std::chrono::time_point<std::chrono::high_resolution_clock> &sceneStartTime,
                             SceneEvent &event)
{
    // Update the ThirdLevelScene scene
    static bool firstMobAlive = true;
    static bool secondMobAlive = true;

    static Entity firstMob = _mobs[0];
    static Entity secondMob = _mobs[1];

    processUserInput(*_manager, sceneStartTime);

    _manager->getRegistry().run_systems();
    auto &healthArray = _manager->getRegistry().get_components<HealthComponent>();

    // Check which mob dies to remove orbs
    if (healthArray[firstMob].has_value() && healthArray[firstMob]->value <= 0)
        firstMobAlive = false;
    if (healthArray[secondMob].has_value() && healthArray[secondMob]->value <= 0)
        secondMobAlive = false;

    thirdLvlMobsAI(*_manager, sceneStartTime, firstMobAlive, secondMobAlive);

    // If no more enemies, next scene
    if (!mobsAlive(_manager->getRegistry()))
    {
        event.event = Event::NEXT;
    }

    StateUpdateMessage state = processOutput(*_manager);
    _manager->pushStateUpdate(state);
}
