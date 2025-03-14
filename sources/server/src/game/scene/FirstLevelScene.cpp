#include "FirstLevelScene.hpp"

#include "EntityTypeComponent.hpp"
#include "EntityUtils.hpp"
#include "SceneManager.hpp"
#include "Systems.hpp"

#include <unistd.h>

using namespace server;

FirstLevelScene::FirstLevelScene(Manager &manager) : AScene(manager) {}

FirstLevelScene::~FirstLevelScene() {}

void FirstLevelScene::enter()
{
    // Add systems
    _manager->getRegistry().add_system<PositionComponent, VelocityComponent>(position_system);
    _manager->getRegistry().add_system<HealthComponent>(health_system);
    _manager->getRegistry().add_system<PositionComponent, VelocityComponent, HealthComponent, EntityTypeComponent>(
        collision_system);
    _manager->getRegistry().add_system<PositionComponent, HealthComponent, EntityTypeComponent>(out_of_bounds_system);
    _manager->getRegistry().add_system<PositionComponent, EntityTypeComponent>(position_wrapping_system);

    restartPlayerPositions(*_manager);
    // createMob(_manager->getRegistry(), {300.0f, 100.0f}, {0.0f, 0.0f}, {100});

    // Add entities
    createMob(_manager->getRegistry(), {1100.0f, 700.0f}, {-0.50f, -1.0f}, {100});
    createMob(_manager->getRegistry(), {1000.0f, 400.0f}, {-1.0f, 1.0f}, {100});
    createMob(_manager->getRegistry(), {900.0f, 300.0f}, {0.1f, 1.50f}, {100});
    createMob(_manager->getRegistry(), {1200.0f, 100.0f}, {0.2f, -1.50f}, {100});
}

void FirstLevelScene::exit()
{
    // Unload the FirstLevelScene scene
    std::cout << "Exiting First Level" << std::endl;
}

void FirstLevelScene::update(const std::chrono::time_point<std::chrono::high_resolution_clock> &sceneStartTime,
                             SceneEvent &event)
{
    // Update the FirstLevelScene scene
    processUserInput(*_manager, sceneStartTime);

    _manager->getRegistry().run_systems();

    // If no more enemies, next scene
    if (!mobsAlive(_manager->getRegistry()))
    {
        event.event = Event::NEXT;
    }

    StateUpdateMessage state = processOutput(*_manager);
    _manager->pushStateUpdate(state);
}
