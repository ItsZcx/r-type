#include "Network.hpp"

#include "Protocol.hpp"

#include <iostream>

using namespace server;

NetworkServer::NetworkServer(asio::io_context &io_context, unsigned short port, Manager &manager)
    : socket_(io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), port)), _manager(manager)
{}

NetworkServer::~NetworkServer()
{
    _running = false;  // Stop the processing thread
    if (_managerProcessingThread.joinable())
    {
        _managerProcessingThread.join();
    }
}

void NetworkServer::start()
{
    std::cout << "Server started, waiting for connections..." << std::endl;
    startReceive();

    // Start a thread to process the Manager's queue
    _managerProcessingThread = std::thread([this]() {
        while (_running)
        {
            processGameOver();
            processManagerQueue();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Check every 100ms
        }
    });
}

void NetworkServer::processGameOver()
{
    auto [isOver, condition] = _manager.getGameOverStatus();
    auto clients = _manager.getClients();

    for (const auto &[clientId, endpoint] : clients_)
    {
        std::vector<uint8_t> buffer;
        GameOverMessage go = {
            {static_cast<uint16_t>(MessageType::GameOver), sizeof(GameOverMessage)},
            clientId,
            condition
        };
        serializeGameOverMessage(go, buffer);
        sendMessage(buffer, endpoint);
    }
}

void NetworkServer::processManagerQueue()
{
    StateUpdateMessage stateMsg;

    // Process state updates from the manager
    while (_manager.popStateUpdate(stateMsg))
    {
        std::cout << "Server processing state update for " << stateMsg.numEntities << " entities from ECS."
                  << std::endl;

        // For now, print the state updates (Later: send to clients)
        for (const auto &entity : stateMsg.entities)
        {
            std::cout << "Entity ID: " << entity.entityId << " PosX: " << entity.posX << " PosY: " << entity.posY
                      << " Health: " << int(entity.health) << std::endl;
        }
        sendGameState(stateMsg);
    }
}

// Handle received messages
void NetworkServer::startReceive()
{
    socket_.async_receive_from(asio::buffer(recv_buffer_), sender_endpoint_,
                               [this](const asio::error_code &error, std::size_t bytes_transferred) {
        handleReceive(error, bytes_transferred);
    });
}

void NetworkServer::handleReceive(const asio::error_code &error, std::size_t bytes_transferred)
{
    if (!error && bytes_transferred > 0)
    {
        std::vector<uint8_t> data(recv_buffer_.begin(), recv_buffer_.begin() + bytes_transferred);
        handleMessage(data, sender_endpoint_);
    } else
    {
        std::cerr << "Error receiving message: " << error.message() << std::endl;
    }
    startReceive();  // Continue receiving
}

// Process the received message (from client)
void NetworkServer::handleMessage(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &sender_endpoint)
{
    MessageHeader header;
    deserializeMessageHeader(data, header);  // get teh message type and size

    // now depending on type  chose what hanlde to use
    switch (static_cast<MessageType>(header.messageType))
    {
        case MessageType::Connect: {
            ConnectMessage connectMsg;
            // fill up connect message and then handle
            deserializeConnectMessage(data, connectMsg);
            handleConnect(connectMsg, sender_endpoint);
            break;
        }
        case MessageType::Disconnect: {
            DisconnectMessage disconnectMsg;
            deserializeDisconnectMessage(data, disconnectMsg);
            handleDisconnect(disconnectMsg, sender_endpoint);
            break;
        }
        case MessageType::UserInput: {
            UserInputMessage userInputMsg;
            deserializeUserInputMessage(data, userInputMsg);
            handleUserInput(userInputMsg);
            break;
        }
        default: std::cerr << "Unknown message type received: " << header.messageType << std::endl; break;
    }
}

// Handle a connection request
void NetworkServer::handleConnect(const ConnectMessage &msg, const asio::ip::udp::endpoint &endpoint)
{
    std::cout << "Client connected with ID: " << msg.clientId << " from " << endpoint << std::endl;

    // Store the client's endpoint
    clients_[msg.clientId] = endpoint;

    // Send a connection acknowledgment back to the client (let client know)
    ConnectMessage ackMsg = {
        {static_cast<uint16_t>(MessageType::Connect), sizeof(ConnectMessage)},
        msg.clientId
    };
    std::vector<uint8_t> buffer;
    serializeConnectMessage(ackMsg, buffer);
    sendMessage(buffer, endpoint);  // back to client the connect succesful..
}

// Handle a disconnect request
void NetworkServer::handleDisconnect(const DisconnectMessage &msg, const asio::ip::udp::endpoint &endpoint)
{
    std::cout << "Client disconnected with ID: " << msg.clientId << " from " << endpoint << std::endl;

    // Remove the client from the map
    clients_.erase(msg.clientId);
}

// Handle user input from a client
void NetworkServer::handleUserInput(const UserInputMessage &msg)
{
    std::cout << "User input received from client ID: " << msg.clientId
              << " with input flags: " << static_cast<int>(msg.inputFlags) << std::endl;
    // TODO: process teh input flags if match withx action and then execute
    if (msg.inputFlags & static_cast<uint8_t>(InputFlags::MoveUp))
    {
        std::cout << "Move Up pressed" << std::endl;
    }
    if (msg.inputFlags & static_cast<uint8_t>(InputFlags::Fire))
    {
        std::cout << "Fire pressed" << std::endl;
    }
    // TODO: Pass input to the ECS system and other inputs to be processed
}

// Placeholder for updating the game state
void NetworkServer::updateGameState()
{
    // TODO: Implement ECS logic
}

// Placeholder for sending the game state to all clients
void NetworkServer::sendGameState(const StateUpdateMessage &stateMsg)
{
    // Serialize the stateMsg into a buffer
    std::vector<uint8_t> buffer;
    serializeStateUpdateMessage(stateMsg, buffer);

    // Send to all clients
    for (const auto &[clientId, endpoint] : clients_)
    {
        sendMessage(buffer, endpoint);
    }
}

// Send a message to a specific client (to the target endpoint)
void NetworkServer::sendMessage(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &target_endpoint)
{
    socket_.async_send_to(asio::buffer(data), target_endpoint,
                          [](const asio::error_code & /*error*/, std::size_t /*bytes_transferred*/) {
        // Handle errors if necessary
    });
}
