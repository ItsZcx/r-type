#include "LobbyScene.hpp"

#include "EntityUtils.hpp"
#include "Manager.hpp"
#include "SceneManager.hpp"

#include <iostream>
#include <random>
#include <unistd.h>

using namespace server;

LobbyScene::LobbyScene(Manager &manager) : AScene(manager), _manager(manager) {}

LobbyScene::~LobbyScene() {}

void LobbyScene::enter()
{
    std::cout << "Entering lobby scene" << std::endl;
}

void LobbyScene::exit()
{
    // Unload the lobby scene
    // this is called one input from client received...
    std::cout << "Exiting lobby scene" << std::endl;
}

void LobbyScene::update(const std::chrono::time_point<std::chrono::high_resolution_clock> &sceneStartTime,
                        SceneEvent &event)
{
    // Update the lobby scene
    std::cout << "Updating lobby scene" << std::endl;
    auto clients = _manager.getClients();
    for (const auto &[clientId, endpoint] : clients)
    {
        if (!_manager.hasEntityForClient(clientId))
        {
            std::cout << "Creating entity for new Client ID: " << clientId << std::endl;
            // create entity player here -> will be only part lobby
            createPlayer(_manager, clientId, {100.0f, 100.0f}, {0.0f, 0.0f}, {200});
        }
    }

    if (clients.empty())
    {
        // No clients, do whatever makes sense (e.g. just continue the loop)
        std::cout << "No clients connected." << std::endl;
        // If you want to skip further logic, just return or continue
        return;  // or `continue;` if you're in a while-loop, etc.
    }

    std::cout << "[ECS] Before catching entity entity:\n";
    // here state update to all clients connected
    auto &posArray = _manager.getRegistry().get_components<PositionComponent>();
    std::cout << "[ECS] After getting pos array:\n";
    auto &velArray = _manager.getRegistry().get_components<VelocityComponent>();
    std::cout << "[ECS] After getting vel array:\n";
    auto &healthArray = _manager.getRegistry().get_components<HealthComponent>();
    std::cout << "[ECS] After getting healt array:\n";
    auto &typeArray = _manager.getRegistry().get_components<EntityTypeComponent>();
    std::cout << "[ECS] After getting type array:\n";

    // --------------------------- DEBUGGING ---------------------- //
    std::cout << "[ECS] Entity States After Update:\n";
    for (size_t i = 0; i < posArray.size(); ++i)
    {
        if (posArray[i].has_value())
        {
            float px = posArray[i]->x;
            float py = posArray[i]->y;

            float vx = 0.0f;
            float vy = 0.0f;
            if (i < velArray.size() && velArray[i].has_value())
            {
                vx = velArray[i]->vx;
                vy = velArray[i]->vy;
            }

            EntityType eType;
            if (i < typeArray.size() && typeArray[i].has_value())
            {
                eType = typeArray[i]->type;
            }

            int hp = -1;
            if (i < healthArray.size() && healthArray[i].has_value())
            {
                hp = healthArray[i]->value;
            }

            std::string entityType = (eType == EntityType::PLAYER)   ? "Player"
                                     : (eType == EntityType::MOB)    ? "Mob"
                                     : (eType == EntityType::BULLET) ? "Bullet"
                                                                     : "Unknown";

            std::cout << "  Entity " << i << " Type:" << entityType << " Pos:(" << px << ", " << py << ")"
                      << " Vel:(" << vx << ", " << vy << ")"
                      << " HP:" << hp << "\n";
        }
    }
    // --------------------------- DEBUGGING ---------------------- //

    // maybe encapsulate here -> but we will have only clients here -> no need to care about pos or so
    // sended just to keep standart
    StateUpdateMessage stateMsg;
    stateMsg.header.messageType = static_cast<uint16_t>(MessageType::StateUpdate);

    std::vector<EntityState> entityStates;
    for (size_t i = 0; i < posArray.size(); ++i)
    {
        if (posArray[i].has_value())
        {
            float px = posArray[i]->x;
            float py = posArray[i]->y;
            float vx = 0.0f;
            float vy = 0.0f;
            EntityType eType;
            uint8_t hp = 0;

            if (i < velArray.size() && velArray[i].has_value())
            {
                vx = velArray[i]->vx;
                vy = velArray[i]->vy;
            }

            if (i < typeArray.size() && typeArray[i].has_value())
            {
                eType = typeArray[i]->type;
            }

            if (i < healthArray.size() && healthArray[i].has_value())
            {
                hp = static_cast<uint8_t>(healthArray[i]->value);
            }

            EntityState es;
            es.entityId = static_cast<uint32_t>(i);
            es.clientId = _manager.getClientIdForEntityId(i);
            es.posX = px;
            es.posY = py;
            es.velX = vx;
            es.velY = vy;
            es.entityType = eType;
            es.health = hp;
            entityStates.push_back(es);
        }
    }

    stateMsg.numEntities = static_cast<uint32_t>(entityStates.size());
    stateMsg.entities = std::move(entityStates);
    stateMsg.header.messageSize =
        sizeof(StateUpdateMessage) + static_cast<uint16_t>(stateMsg.entities.size() * sizeof(EntityState));

    // to manager see when update
    _manager.pushStateUpdate(stateMsg);

    // read client input here
    bool start_game = processStartGame(_manager);
    // TODO -> Next thing to test here
    // processKickPlayer(_manager);
    if (start_game)
    {
        event.event = Event::NEXT;
    }
}
