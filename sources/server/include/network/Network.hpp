#ifndef NETWORK_HPP
#define NETWORK_HPP

#include "Manager.hpp"
#include "Protocol.hpp"

#include <asio.hpp>
#include <unordered_map>
#include <vector>

namespace server
{

class NetworkServer
{
  public:
    // Constructor
    NetworkServer(asio::io_context &io_context, unsigned short port, Manager &manager);
    ~NetworkServer();  // Explicitly declare the destructor

    // Public methods
    void start();  // Start the server and begin listening
    // ! manager state queue
    void processManagerQueue();  // Periodically process the manager's state update queue
    void processGameOver();
    void updateGameState();                                  // To be implemented with ECS integration
    void sendGameState(const StateUpdateMessage &stateMsg);  // To send game state to clients

    // public allow the manager to use this form server -> to add to quequs when happen
    void virtual handleUserInput(const UserInputMessage &msg);
    void virtual handleConnect(const ConnectMessage &msg, const asio::ip::udp::endpoint &endpoint);
    void virtual handleDisconnect(const DisconnectMessage &msg, const asio::ip::udp::endpoint &endpoint);

  private:
    // Internal utility functions
    void startReceive();                                                               // Begin asynchronous receive
    void handleReceive(const asio::error_code &error, std::size_t bytes_transferred);  // Handle incoming messages
    void handleMessage(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &sender_endpoint);

    // Send utility
    void sendMessage(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &target_endpoint);

    // Server state
    asio::ip::udp::socket socket_;
    asio::ip::udp::endpoint sender_endpoint_;
    std::array<uint8_t, 1024> recv_buffer_;

    std::unordered_map<uint32_t, asio::ip::udp::endpoint> clients_;  // Map of connected clients (clientId -> endpoint)

    // manager related -> be able to get the manager info on the state queue
    Manager &_manager;                     // Reference to the manager
    std::thread _managerProcessingThread;  // Thread for processing the manager queue
    bool _running = true;                  // Control flag for the processing thread
};

}  // namespace server

#endif  // NETWORK_HPP
