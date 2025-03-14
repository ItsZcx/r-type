#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include "utils/entity_type.hpp"

#include <asio.hpp>
#include <cstdint>
#include <vector>

// Ensure no padding in structures
#pragma pack(push, 1)

namespace client
{

// Message types
enum class MessageType : uint16_t
{
    Connect = 1,
    Disconnect = 2,
    StateUpdate = 3,
    UserInput = 4,
    GameOver = 5
};

enum class GameOverType : uint16_t
{
    None = 1,
    Win = 2,
    Lose = 3,
};

// Common message header
struct MessageHeader
{
    uint16_t messageType;  // Type of the message
    uint16_t messageSize;  // Total size of the message (header + body)
};

// Connect message (Client to Server)
struct ConnectMessage
{
    MessageHeader header;
    uint32_t clientId;  // Unique ID of the client
};

// Disconnect message (Client to Server)
struct DisconnectMessage
{
    MessageHeader header;
    uint32_t clientId;  // Unique ID of the client
};

// send to all clients at the same type
struct GameOverMessage
{
    MessageHeader header;
    uint32_t clientId;  // Unique ID of the client
    GameOverType condition;
};

// TODO complient wiht the ECS -> what sended to the manager or to the client
struct EntityState
{
    uint32_t clientId;      // client id 42 if not clientId
    uint32_t entityId;      // Unique ID of the entity
    float posX;             // X position
    float posY;             // Y position
    float velX;             // X velocity
    float velY;             // Y velocity
    EntityType entityType;  // Type of the entity (e.g., player, enemy, bullet)
    uint8_t health;         // Health of the entity (see if needed)
};

struct StateUpdateMessage
{
    MessageHeader header;
    uint32_t numEntities;               // Number of entities in the game
    std::vector<EntityState> entities;  // List of entity states
};

// ! revise this (inputs from the user)
enum class InputFlags : uint8_t
{
    MoveUp = 1 << 0,
    MoveDown = 1 << 1,
    MoveLeft = 1 << 2,
    MoveRight = 1 << 3,
    Fire = 1 << 4,

    // LOBBY  FLAGS
    StartGame = 1 << 5,
    // ! The id associated with the inputMessage when KickPlayer flag enabled is the client to be kicked id NOT the current client ID.
    KickPlayer = 1 << 6
};

struct UserInputMessage
{
    MessageHeader header;
    uint32_t clientId;   // Unique ID of the client
    uint8_t inputFlags;  // Combined input flags
};

#pragma pack(pop)

// Serialization and deserialization functions for each message and general header
void serializeMessageHeader(const MessageHeader &header, std::vector<uint8_t> &buffer);
void serializeConnectMessage(const ConnectMessage &msg, std::vector<uint8_t> &buffer);
void serializeDisconnectMessage(const DisconnectMessage &msg, std::vector<uint8_t> &buffer);
void serializeStateUpdateMessage(const StateUpdateMessage &msg, std::vector<uint8_t> &buffer);
void serializeUserInputMessage(const UserInputMessage &msg, std::vector<uint8_t> &buffer);

void deserializeMessageHeader(const std::vector<uint8_t> &buffer, MessageHeader &header);
void deserializeConnectMessage(const std::vector<uint8_t> &buffer, ConnectMessage &msg);
void deserializeDisconnectMessage(const std::vector<uint8_t> &buffer, DisconnectMessage &msg);
void deserializeStateUpdateMessage(const std::vector<uint8_t> &buffer, StateUpdateMessage &msg);
void deserializeUserInputMessage(const std::vector<uint8_t> &buffer, UserInputMessage &msg);

// make the function to serialize and deseralize here and send to all the clients same way we do but not pushed quee direclyt from the manager
// function manager to game over...
// new
void deserializeGameOverMessage(const std::vector<uint8_t> &buffer, GameOverMessage &msg);

}  // namespace client

#endif  // PROTOCOL_HPP
