#ifndef MANAGER_HPP
#define MANAGER_HPP

#include "Entity.hpp"    // Include your Entity type
#include "Protocol.hpp"  // For the message types
#include "Registry.hpp"  // Include your Registry header

#include <asio.hpp>
#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <queue>
#include <unordered_map>

namespace server
{

class Manager
{
  public:
    // Input handling
    void pushInput(const UserInputMessage &msg);
    bool popInput(UserInputMessage &msg);

    // State update handling
    void pushStateUpdate(const StateUpdateMessage &msg);
    bool popStateUpdate(StateUpdateMessage &msg);

    // Client handling
    void addClient(uint32_t clientId, const asio::ip::udp::endpoint &endpoint);
    void removeClient(uint32_t clientId);
    std::unordered_map<uint32_t, asio::ip::udp::endpoint> getClients();

    // ECS access
    // Returns the Registry reference so ECS systems can be used
    Registry &getRegistry();

    // Entity <-> Client mapping
    void mapClientToEntity(uint32_t clientId, Entity entity);
    bool hasEntityForClient(uint32_t clientId);
    Entity getEntityForClient(uint32_t clientId);
    uint32_t getClientIdForEntityId(size_t entityId);

    std::pair<bool, GameOverType> getGameOverStatus();

    // 2) A setter to change the game over status + type
    void setGameOverStatus(bool isOver, GameOverType type);

  private:
    // Input queue
    std::queue<UserInputMessage> _inputQueue;
    std::mutex _inputMutex;
    std::condition_variable _inputCV;

    // State queue
    std::queue<StateUpdateMessage> _stateQueue;
    std::mutex _stateMutex;
    std::condition_variable _stateCV;

    // Connected clients
    std::unordered_map<uint32_t, asio::ip::udp::endpoint> _clients;
    std::mutex _clientsMutex;

    // var that is shared
    // The ECS Registry
    Registry _registry;

    // Mapping from clientId to the Entity that represents the player
    std::unordered_map<uint32_t, Entity> _clientToEntityMap;

    std::mutex _entityMapMutex;
    // new entity id to clinet
    std::unordered_map<size_t, uint32_t> _entityIdToClientId;
    std::mutex _clientIdMapMutex;

    std::mutex _gameOverMutex;
    bool _isGameOver {false};
    GameOverType _gameOverType {GameOverType::None};  // Default or pick whichever
};

}  // namespace server

#endif  // MANAGER_HPP
