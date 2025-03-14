#include "Server.hpp"

#include "BossLevelScene.hpp"
#include "EntityUtils.hpp"
#include "ExtendedNetwork.hpp"
#include "FirstLevelScene.hpp"
#include "LobbyScene.hpp"
#include "Manager.hpp"
#include "SceneManager.hpp"
#include "SecondLevelScene.hpp"
#include "Server.hpp"
#include "ThirdLevelScene.hpp"

#include <asio.hpp>
#include <cstddef>
#include <iostream>
#include <memory>
#include <thread>

using namespace server;

Server::Server(unsigned short port) : _port(port) {}
Server::~Server() {}

bool gameOver(Manager &manager, SceneManager &sceneManager)
{
    if (sceneManager.currentSceneIdx() == sceneManager.size())
    {
        // send win game over
        // funcion manager to send the state to the server
        manager.setGameOverStatus(true, GameOverType::Win);
        return true;
    } else if (sceneManager.currentSceneIdx() != 0 && countPlayers(manager.getRegistry()) == 0)
    {
        // send lose game over
        manager.setGameOverStatus(true, GameOverType::Lose);
        return true;
    }
    return false;
}

void gameLoop(Manager &manager)
{
    size_t sceneIdx = 0;
    SceneManager sceneManager(manager);

    // Create scenes and add them to the scene manager
    std::unique_ptr<LobbyScene> lobbyScene = std::make_unique<LobbyScene>(manager);
    std::unique_ptr<FirstLevelScene> firstLevelScene = std::make_unique<FirstLevelScene>(manager);
    std::unique_ptr<SecondLevelScene> secondLevelScene = std::make_unique<SecondLevelScene>(manager);
    std::unique_ptr<ThirdLevelScene> thirdLevelScene = std::make_unique<ThirdLevelScene>(manager);
    std::unique_ptr<BossLevelScene> bossLevelScene = std::make_unique<BossLevelScene>(manager);

    sceneManager.addScene(std::move(lobbyScene));
    sceneManager.addScene(std::move(firstLevelScene));
    sceneManager.addScene(std::move(secondLevelScene));
    sceneManager.addScene(std::move(thirdLevelScene));
    sceneManager.addScene(std::move(bossLevelScene));

    // mapped to enter scene??
    sceneManager.atScene(0);

    auto scenetStartTime = std::chrono::high_resolution_clock::now();

    while (true)
    {
        if (sceneIdx != sceneManager.currentSceneIdx())
            scenetStartTime = std::chrono::high_resolution_clock::now();

        // mapped to scene update func
        sceneManager.update(scenetStartTime);

        sceneIdx = sceneManager.currentSceneIdx();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));

        if (gameOver(manager, sceneManager))
            break;
    }
}

void Server::run()
{
    try
    {
        asio::io_context io_context;

        // Create the Manager
        Manager manager;

        // Start the Extended Network Server
        ExtendedNetworkServer server(io_context, _port, manager);
        server.start();

        // Run the network logic in a separate thread
        std::thread serverThread([&io_context]() { io_context.run(); });

        // Run the ECS system in another thread -> have the ecsLoop
        std::thread ecsThread([&manager]() { gameLoop(manager); });

        // At this point, the server is running and the ECS loop is running.
        // The main thread doesn't simulate a client anymore; it just waits.

        std::cout << "Server is running on port " << _port << ". Press Ctrl+C to stop." << std::endl;

        // Wait indefinitely until process is terminated
        // Alternatively, you could implement a command loop or a signal handler for a clean shutdown.
        serverThread.join();

        // If server is stopped, ecsLoop might be ended by externally stopping io_context or another trigger.
        ecsThread.join();

        std::cout << "Server and ECS shutdown completed!" << std::endl;

    } catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
