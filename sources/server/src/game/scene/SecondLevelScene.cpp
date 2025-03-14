#include "SecondLevelScene.hpp"

#include "EntityUtils.hpp"
#include "SceneManager.hpp"

#include <unistd.h>

using namespace server;

SecondLevelScene::SecondLevelScene(Manager &manager) : AScene(manager) {}

SecondLevelScene::~SecondLevelScene() {}

void SecondLevelScene::enter()
{
    restartPlayerPositions(*_manager);

    // Add entities
    createMob(_manager->getRegistry(), {1000.0f, 50.0f}, {-4.0f, 0.0f}, {100});
    createMob(_manager->getRegistry(), {1000.0f, 250.0f}, {-4.0f, 0.0f}, {100});
    createMob(_manager->getRegistry(), {1000.0f, 450.0f}, {-4.0f, 0.0f}, {100});
    createMob(_manager->getRegistry(), {1000.0f, 650.0f}, {-4.0f, 0.0f}, {100});
    createMob(_manager->getRegistry(), {1000.0f, 850.0f}, {-4.0f, 0.0f}, {100});
    createMob(_manager->getRegistry(), {1000.0f, 1050.0f}, {-4.0f, 0.0f}, {100});
}

void SecondLevelScene::exit()
{
    // Unload the SecondLevelScene scene
    std::cout << "Exiting Second Level" << std::endl;
}

void SecondLevelScene::update(const std::chrono::time_point<std::chrono::high_resolution_clock> &sceneStartTime,
                              SceneEvent &event)
{
    // Update the SecondLevelScene scene
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
