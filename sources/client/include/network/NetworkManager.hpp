/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** NetworkManager
*/

#ifndef NETWORKMANAGER_HPP
#define NETWORKMANAGER_HPP

#include "network/Protocol.hpp"
#include "utils/dotenv.h"

#include <asio.hpp>
#include <iostream>
#include <mutex>
#include <optional>
#include <queue>
#include <random>
#include <thread>
#include <unordered_map>

#define MAX_UPDATES_PER_CYCLE 5

#define MAX_MESSAGE_SIZE 2308

namespace client
{

class NetworkManager
{
  public:
    using StateUpdateCallback = std::function<void(const StateUpdateMessage &)>;
    using GameOverCallback = std::function<void(const GameOverMessage &)>;

    NetworkManager(float &deltaTime);
    ~NetworkManager();

    // Public methods
    void connectToServer();
    void disconnectFromServer();
    void sendUserInput(uint8_t inputFlags);
    void startReceive();
    void run();
    void setStateUpdateCallback(StateUpdateCallback callback);
    void setGameOverCallback(GameOverCallback callback);

    // New public methods for update queue
    bool hasPendingUpdates() const;
    std::optional<StateUpdateMessage> getNextUpdate();

    uint32_t getClientId() const;

    void toUpdate();

    void setIpPort(const std::string &ipPort);

  private:
    // Private utility methods
    void _handleReceive(const asio::error_code &error, std::size_t bytes_transferred);
    void _processReceivedMessage(const std::vector<uint8_t> &data);
    void _handleStateUpdate(const StateUpdateMessage &stateMsg);
    uint32_t _generateClientId();
    void _processStateUpdate(const StateUpdateMessage &stateMsg);

    // ASIO components
    asio::io_context _io_context;
    asio::ip::udp::socket _clientSocket;
    asio::ip::udp::endpoint _serverEndpoint;
    std::vector<uint8_t> _recv_buffer;

    // State variables
    uint32_t _clientId;
    std::atomic<bool> _isConnected;
    std::thread _receiveThread;

    StateUpdateCallback _stateUpdateCallback;
    GameOverCallback _gameOverCallback;

    // New variables for update queue
    mutable std::mutex _queueMutex;
    std::queue<StateUpdateMessage> _updateQueue;
    static constexpr size_t MAX_QUEUE_SIZE = 20;

    // variables for updates control
    float _updateInterval;
    float _updateTimer;
    float &_deltaTime;
    bool _firstUpdate = true;
    bool _update = false;
};

}  // namespace client

#endif  // NETWORKMANAGER_HPP
