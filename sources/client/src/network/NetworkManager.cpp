#include "network/NetworkManager.hpp"

#include "network/Protocol.hpp"
#include "utils/dotenv.h"

using namespace client;

/**
 * @brief Constructs a NetworkManager and initializes networking components.
 * 
 * The server endpoint is set to the values from the .env file.
 */
NetworkManager::NetworkManager(float &deltaTime)
    : _clientSocket(_io_context), _recv_buffer(2308), _isConnected(false), _updateInterval(0.016f), _updateTimer(0.0f),
      _deltaTime(deltaTime), _update(true)
{
    // dotenv::init();
    // std::string host = dotenv::getenv("SERVER_HOST");
    // std::string port = dotenv::getenv("SERVER_PORT");

    // _serverEndpoint = asio::ip::udp::endpoint(asio::ip::address::from_string(host), std::stoi(port));
}

/**
 * @brief Destructor to clean up resources.
 */
NetworkManager::~NetworkManager()
{
    _isConnected = false;

    if (_receiveThread.joinable())
    {
        // std::cout << "Waiting for receive thread to finish..." << std::endl;
        _receiveThread.join();
    }

    _io_context.stop();
    // std::cout << "NetworkManager destroyed." << std::endl;
}

/**
 * @brief Connects to the server and sends a ConnectMessage.
 * 
 * The client ID is generated and stored in the _clientId member variable.
 * The client socket is opened and a ConnectMessage is sent to the server.
 * The _isConnected flag is set to true.
 * The startReceive method is called to start receiving messages from the server.
 * A new thread is created to run the Asio I/O context.
 */
void NetworkManager::connectToServer()
{
    _clientId = _generateClientId();

    // Open the UDP socket
    _clientSocket.open(asio::ip::udp::v4());

    // Send ConnectMessage
    ConnectMessage connectMsg = {
        {static_cast<uint16_t>(MessageType::Connect), sizeof(ConnectMessage)},
        _clientId
    };

    std::vector<uint8_t> buffer;
    serializeConnectMessage(connectMsg, buffer);

    _clientSocket.send_to(asio::buffer(buffer), _serverEndpoint);

    _isConnected = true;

    // Start receiving messages
    startReceive();

    _receiveThread = std::thread([this]() { run(); });
    // std::cout << "Connected to server with clientId: " << clientId_ << std::endl;
}

/**
 * @brief Disconnects from the server and sends a DisconnectMessage.
 * 
 * The client socket is closed and a DisconnectMessage is sent to the server.
 * The _isConnected flag is set to false.
 * The client socket is closed.
 */
void NetworkManager::disconnectFromServer()
{
    if (_isConnected)
    {
        // std::cout << "Disconnecting from server..." << std::endl;
        DisconnectMessage disconnectMsg = {
            {static_cast<uint16_t>(MessageType::Disconnect), sizeof(DisconnectMessage)},
            _clientId
        };

        std::vector<uint8_t> buffer;
        serializeDisconnectMessage(disconnectMsg, buffer);

        _clientSocket.send_to(asio::buffer(buffer), _serverEndpoint);

        _isConnected = false;
        _clientSocket.close();
        // std::cout << "Disconnected from server." << std::endl;
    } else
    {
        // std::cout << "Already disconnected." << std::endl;
    }
}

/**
 * @brief Sends user input as a UserInputMessage to the server.
 * @param inputFlags Flags representing user inputs.
 */
void NetworkManager::sendUserInput(uint8_t inputFlags)
{
    if (!_isConnected)
        return;

    UserInputMessage inputMsg = {
        {static_cast<uint16_t>(MessageType::UserInput), sizeof(UserInputMessage)},
        _clientId,
        static_cast<uint8_t>(inputFlags)
    };

    std::vector<uint8_t> buffer;
    serializeUserInputMessage(inputMsg, buffer);

    // std::cout << "Sending input flags: " << static_cast<uint8_t>(inputFlags) << std::endl;

    _clientSocket.send_to(asio::buffer(buffer), _serverEndpoint);
}

/**
 * @brief Starts the asynchronous reception of messages from the server.
 * This method is called recursively to keep receiving messages.
 * The received messages are processed by the _processReceivedMessage method.
 */
void NetworkManager::startReceive()
{
    if (_recv_buffer.size() < MAX_MESSAGE_SIZE)
    {
        std::cerr << "Warning: recv_buffer_ size (" << _recv_buffer.size()
                  << " bytes) is smaller than the maximum expected message size (" << MAX_MESSAGE_SIZE << " bytes)."
                  << std::endl;
    }

    _clientSocket.async_receive_from(asio::buffer(_recv_buffer), _serverEndpoint,
                                     [this](const asio::error_code &error, std::size_t bytesReceived) {
        if (!error && bytesReceived > 0)
        {
            std::vector<uint8_t> data(_recv_buffer.begin(), _recv_buffer.begin() + bytesReceived);
            _processReceivedMessage(data);
        } else if (error)
        {
            std::cerr << "Error receiving message: " << error.message() << std::endl;
        }

        if (_isConnected)
        {
            startReceive();
        }
    });

    // std::cout << "Receive thread stopped." << std::endl;
}

/**
 * @brief Processes received messages based on their type.
 * @param data The buffer containing the received message.
 */
void NetworkManager::_processReceivedMessage(const std::vector<uint8_t> &data)
{
    MessageHeader header;
    deserializeMessageHeader(data, header);

    // std::cout << "Processing message with type: " << header.messageType << ", size: " << header.messageSize
    //           << std::endl;

    switch (static_cast<MessageType>(header.messageType))
    {
        case MessageType::Connect: {
            ConnectMessage connectMsg;
            deserializeConnectMessage(data, connectMsg);
            // std::cout << "Received connect message from client ID: " << connectMsg.clientId << std::endl;
            break;
        }
        case MessageType::StateUpdate: {
            std::cout << "Received StateUpdateMessage!" << std::endl;
            StateUpdateMessage stateMsg;
            deserializeStateUpdateMessage(data, stateMsg);
            // std::cout << "Update message received" << std::endl;

            _handleStateUpdate(stateMsg);
            break;
        }
        case MessageType::GameOver: {
            GameOverMessage gameOverMsg;
            deserializeGameOverMessage(data, gameOverMsg);
            std::cout << "Received game over message!" << std::endl;
            if (_gameOverCallback)
            {
                _gameOverCallback(gameOverMsg);
            }
            break;
        }
        default: std::cerr << "Unknown message type received: " << header.messageType << std::endl; break;
    }
}

void NetworkManager::setStateUpdateCallback(StateUpdateCallback callback)
{
    _stateUpdateCallback = callback;
}

void NetworkManager::setGameOverCallback(GameOverCallback callback)
{
    _gameOverCallback = callback;
}

/**
 * @brief Handles a StateUpdateMessage by updating the local game state.
 * @param stateMsg The state update message from the server.
 */
void NetworkManager::_handleStateUpdate(const StateUpdateMessage &stateMsg)
{
    // std::cout << "Received state update for " << stateMsg.numEntities << " entities." << std::endl;

    //output state update message

    // if (_update)
    // {
    //     std::cout << "Header - MessageType: " << stateMsg.header.messageType << std::endl;
    //     _update = false;
    // }

    for (const auto &entityState : stateMsg.entities)
    {
        // std::cout << "Entity ID: " << entityState.entityId << std::endl;
        // if (static_cast<EntityType>(entityState.entityType) == EntityType::PLAYER)
        // {
        //     std::cout << "Entity ID: " << entityState.entityId << ", Type: " << static_cast<uint16_t>(entityState.entityType)
        //               << ", Position: (" << entityState.posX << ", " << entityState.posY << ")"
        //               << ", Velocity: (" << entityState.velX << ", " << entityState.velY << ")"
        //               << ", Health: " << static_cast<int>(entityState.health) << ", ClientId: " << entityState.clientId
        //               << std::endl;
        //     std::cout << "Current Player Id: " << _clientId << std::endl;
        // }
    }

    if (_stateUpdateCallback /* && _update */)
    {
        _stateUpdateCallback(stateMsg);
        _update = false;
    }
}

/**
 * @brief Runs the Asio I/O context.
 */
void NetworkManager::run()
{
    try
    {
        _io_context.run();
    } catch (const std::exception &e)
    {
        std::cerr << "Run error: " << e.what() << std::endl;
    }
}

/**
 * Creates a random client ID, using a Mersenne Twister random number generator.
 * 
 * @return The generated client ID.
 */
uint32_t NetworkManager::_generateClientId()
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<uint32_t> dis(1, UINT32_MAX);

    uint32_t clientId = dis(gen);

    return clientId;
}

void NetworkManager::toUpdate()
{
    _update = true;
}

uint32_t NetworkManager::getClientId() const
{
    return _clientId;
}

void NetworkManager::setIpPort(const std::string &ipPort)
{
    std::string host;
    std::string port;

    size_t pos = ipPort.find(':');
    if (pos != std::string::npos)
    {
        host = ipPort.substr(0, pos);
        port = ipPort.substr(pos + 1);
    } else
    {
        std::cerr << "Invalid IP:Port format." << std::endl;
        return;
    }

    _serverEndpoint = asio::ip::udp::endpoint(asio::ip::address::from_string(host), std::stoi(port));
}
