#include "Manager.hpp"

#include <cstdint>

using namespace server;

void Manager::setGameOverStatus(bool isOver, GameOverType type)
{
    std::lock_guard<std::mutex> lock(_gameOverMutex);
    _isGameOver = isOver;
    _gameOverType = type;
}

std::pair<bool, GameOverType> Manager::getGameOverStatus()
{
    std::lock_guard<std::mutex> lock(_gameOverMutex);
    // Return both bool and GameOverType as a pair
    return std::make_pair(_isGameOver, _gameOverType);
}

void Manager::pushInput(const UserInputMessage &msg)
{
    std::lock_guard<std::mutex> lock(_inputMutex);
    _inputQueue.push(msg);
    _inputCV.notify_one();
}

bool Manager::popInput(UserInputMessage &msg)
{
    std::lock_guard<std::mutex> lock(_inputMutex);
    if (_inputQueue.empty())
        return false;
    msg = _inputQueue.front();
    _inputQueue.pop();
    return true;
}

void Manager::pushStateUpdate(const StateUpdateMessage &msg)
{
    std::lock_guard<std::mutex> lock(_stateMutex);
    _stateQueue.push(msg);
    _stateCV.notify_one();
}

bool Manager::popStateUpdate(StateUpdateMessage &msg)
{
    std::lock_guard<std::mutex> lock(_stateMutex);
    if (_stateQueue.empty())
        return false;
    msg = _stateQueue.front();
    _stateQueue.pop();
    return true;
}

void Manager::addClient(uint32_t clientId, const asio::ip::udp::endpoint &endpoint)
{
    std::lock_guard<std::mutex> lock(_clientsMutex);
    _clients[clientId] = endpoint;
}

void Manager::removeClient(uint32_t clientId)
{
    {
        std::lock_guard<std::mutex> lock(_clientsMutex);
        _clients.erase(clientId);
    }
    {
        std::lock_guard<std::mutex> lock(_entityMapMutex);
        _clientToEntityMap.erase(clientId);
    }
}

std::unordered_map<uint32_t, asio::ip::udp::endpoint> Manager::getClients()
{
    std::lock_guard<std::mutex> lock(_clientsMutex);
    return _clients;
}

// Return the registry so it can be accessed by ECS loop -> create the registry on the manager to get accesed
Registry &Manager::getRegistry()
{
    return _registry;
}

void Manager::mapClientToEntity(uint32_t clientId, Entity entity)
{
    std::lock_guard<std::mutex> lock(_entityMapMutex);
    _clientToEntityMap[clientId] = entity;  // Now works because 'Entity' is default-constructible
    _entityIdToClientId[entity._id] = clientId;
}

bool Manager::hasEntityForClient(uint32_t clientId)
{
    std::lock_guard<std::mutex> lock(_entityMapMutex);  // ensure entityMapMutex is not const
    return (_clientToEntityMap.find(clientId) != _clientToEntityMap.end());
}

Entity Manager::getEntityForClient(uint32_t clientId)
{
    std::lock_guard<std::mutex> lock(_entityMapMutex);  // ensure entityMapMutex is not const
    auto it = _clientToEntityMap.find(clientId);
    if (it != _clientToEntityMap.end())
    {
        return it->second;
    }
    // return ivnalid entity
    return Entity(0);
}

uint32_t Manager::getClientIdForEntityId(size_t entityId)
{
    std::lock_guard<std::mutex> lock(_clientIdMapMutex);  // ensure entityMapMutex is not const
    auto it = _entityIdToClientId.find(entityId);
    if (it != _entityIdToClientId.end())
    {
        return it->second;
    }
    // return ivnalid entity -> 42 not client
    return 42;
}
