#ifndef EXTENDED_NETWORK_HPP
#define EXTENDED_NETWORK_HPP

#include "Manager.hpp"
#include "Network.hpp"

namespace server
{

class ExtendedNetworkServer : public NetworkServer
{
  public:
    ExtendedNetworkServer(asio::io_context &io_context, unsigned short port, Manager &manager)
        : NetworkServer(io_context, port, manager), _manager(manager)
    {}

    void handleUserInput(const UserInputMessage &msg) override
    {
        NetworkServer::handleUserInput(
            msg);  // handle user input that this is tranform the message -> but then we puss it to the manager
        _manager.pushInput(msg);  // Push user input to the manager
    }

    // connect plus add to the manager
    void handleConnect(const ConnectMessage &msg, const asio::ip::udp::endpoint &endpoint) override
    {
        NetworkServer::handleConnect(msg, endpoint);
        _manager.addClient(msg.clientId, endpoint);  // Add client to the manager
    }

    // disconnect plus remove from the manager
    void handleDisconnect(const DisconnectMessage &msg, const asio::ip::udp::endpoint &endpoint) override
    {
        NetworkServer::handleDisconnect(msg, endpoint);
        _manager.removeClient(msg.clientId);  // Remove client from the manager
    }

  private:
    Manager &_manager;
};

}  // namespace server

#endif  // EXTENDED_NETWORK_HPP
