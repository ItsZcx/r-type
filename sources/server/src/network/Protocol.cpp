#include "Protocol.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>

using namespace server;

// Serialize the common message header (header and buffer)
void server::serializeMessageHeader(const MessageHeader &header, std::vector<uint8_t> &buffer)
{
    // insert -> pos, the firstm and last (is type + size of type...) reintrpret pointer to be uint8_t pointer to have [(byte1, byte2), (byte3, byte4)] type and size
    uint16_t type = htons(header.messageType);
    uint16_t size = htons(header.messageSize);

    // we crete a buffer with the data -> we first insert the type and then the size
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&type),
                  reinterpret_cast<const uint8_t *>(&type) + sizeof(type));

    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&size),
                  reinterpret_cast<const uint8_t *>(&size) + sizeof(size));
}

// Serialize ConnectMessage
void server::serializeConnectMessage(const ConnectMessage &msg, std::vector<uint8_t> &buffer)
{
    server::serializeMessageHeader(msg.header, buffer);

    uint32_t clientId = htonl(msg.clientId);
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&clientId),
                  reinterpret_cast<const uint8_t *>(&clientId) + sizeof(clientId));
}

// Serialize DisconnectMessage
void server::serializeDisconnectMessage(const DisconnectMessage &msg, std::vector<uint8_t> &buffer)
{
    server::serializeMessageHeader(msg.header, buffer);

    // we use equivalent to pushback.. always insert at the end
    uint32_t clientId = htonl(msg.clientId);
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&clientId),
                  reinterpret_cast<const uint8_t *>(&clientId) + sizeof(clientId));
}

void server::serializeGameOverMessage(const GameOverMessage &msg, std::vector<uint8_t> &buffer)
{
    // First serialize the common message header (4 bytes total)
    server::serializeMessageHeader(msg.header, buffer);

    uint32_t netClientId = htonl(msg.clientId);
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&netClientId),
                  reinterpret_cast<const uint8_t *>(&netClientId) + sizeof(netClientId));

    uint16_t netCondition = htons(static_cast<uint16_t>(msg.condition));
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&netCondition),
                  reinterpret_cast<const uint8_t *>(&netCondition) + sizeof(netCondition));
}

// Serialize StateUpdateMessage
void server::serializeStateUpdateMessage(const StateUpdateMessage &msg, std::vector<uint8_t> &buffer)
{
    server::serializeMessageHeader(msg.header, buffer);

    uint32_t numEntities = htonl(msg.numEntities);
    buffer.insert(buffer.end(),
                  reinterpret_cast<const uint8_t *>(
                      &numEntities),  // start the position of the ponter num entities an den that + the size
                  reinterpret_cast<const uint8_t *>(&numEntities) + sizeof(numEntities));

    for (const auto &entity : msg.entities)
    {
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&entity),
                      reinterpret_cast<const uint8_t *>(&entity) + sizeof(EntityState));
    }
}

// !THIS IS A EXAMPLE FOR THE CLINENT

// Example  on how to send input message... (construct header + msessage and join the flags via bitwise OR)
// UserInputMessage inputMsg;
// inputMsg.header.messageType = static_cast<uint16_t>(MessageType::UserInput);
// inputMsg.header.messageSize = sizeof(UserInputMessage);
// inputMsg.clientId = clientId;
// inputMsg.inputFlags = static_cast<uint8_t>(InputFlags::MoveUp) | static_cast<uint8_t>(InputFlags::Fire);

void server::serializeUserInputMessage(const UserInputMessage &msg, std::vector<uint8_t> &buffer)
{
    server::serializeMessageHeader(msg.header, buffer);

    uint32_t clientId = htonl(msg.clientId);
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&clientId),
                  reinterpret_cast<const uint8_t *>(&clientId) + sizeof(clientId));

    buffer.push_back(msg.inputFlags);
}

// ! Deserialize the common message header -> used by the client and server

// Deserialize MessageHeader
void server::deserializeMessageHeader(const std::vector<uint8_t> &buffer, MessageHeader &header)
{
    if (buffer.size() < sizeof(MessageHeader))
        throw std::runtime_error("Buffer too small for header");

    memcpy(&header, buffer.data(), sizeof(MessageHeader));  // copy the first 4 bytes to the header type
    // htons is int to bytes, ntohs is bytes to int
    header.messageType = ntohs(header.messageType);  // nthons network to host short bytes to int
    header.messageSize = ntohs(header.messageSize);
}

// Deserialize ConnectMessage -> take the bytes and memcopp to the structe sru dta an sizeof the
// the src is the startign point for that so the point  buffer + already filled
void server::deserializeConnectMessage(const std::vector<uint8_t> &buffer, ConnectMessage &msg)
{
    server::deserializeMessageHeader(buffer, msg.header);

    if (buffer.size() < sizeof(ConnectMessage))
        throw std::runtime_error("Buffer too small for ConnectMessage");

    // read from data + 4 bytes (header) the next 4 bytes (client id)
    memcpy(&msg.clientId, buffer.data() + sizeof(MessageHeader), sizeof(uint32_t));
    msg.clientId = ntohl(msg.clientId);
}

// Deserialize DisconnectMessage
void server::deserializeDisconnectMessage(const std::vector<uint8_t> &buffer, DisconnectMessage &msg)
{
    server::deserializeMessageHeader(buffer, msg.header);

    if (buffer.size() < sizeof(DisconnectMessage))
        throw std::runtime_error("Buffer too small for DisconnectMessage");

    memcpy(&msg.clientId, buffer.data() + sizeof(MessageHeader), sizeof(uint32_t));
    msg.clientId = ntohl(msg.clientId);
}

// Deserialize StateUpdateMessage (CLIENT)
void server::deserializeStateUpdateMessage(const std::vector<uint8_t> &buffer, StateUpdateMessage &msg)
{
    server::deserializeMessageHeader(buffer, msg.header);

    if (buffer.size() < sizeof(MessageHeader) + sizeof(uint32_t))  // each state has the num of entites
        throw std::runtime_error("Buffer too small for StateUpdateMessage");

    memcpy(&msg.numEntities, buffer.data() + sizeof(MessageHeader), sizeof(uint32_t));
    msg.numEntities = ntohl(msg.numEntities);

    size_t offset = sizeof(MessageHeader) + sizeof(uint32_t);  // header + num of entiites
    for (uint32_t i = 0; i < msg.numEntities; ++i)
    {
        if (offset + sizeof(EntityState) > buffer.size())
            throw std::runtime_error("Buffer too small for EntityState");

        EntityState entity;
        memcpy(&entity, buffer.data() + offset, sizeof(EntityState));
        msg.entities.push_back(entity);  // push_back to the entities vector -> (will be client thing this)
        offset += sizeof(EntityState);
    }
}

// Deserialize UserInputMessage SERVER
void server::deserializeUserInputMessage(const std::vector<uint8_t> &buffer, UserInputMessage &msg)
{
    server::deserializeMessageHeader(buffer, msg.header);

    if (buffer.size() < sizeof(MessageHeader) + sizeof(uint32_t) + sizeof(uint8_t))
        throw std::runtime_error("Buffer too small for UserInputMessage");

    memcpy(&msg.clientId, buffer.data() + sizeof(MessageHeader), sizeof(uint32_t));
    msg.clientId = ntohl(msg.clientId);

    // could use this but since is the last byte we just get the last byte of the buffer
    // memcpy(&msg.inputFlags,buffer.data() + sizeof(MessageHeader) + sizeof(uint32_t), sizeof(uint8_t));
    msg.inputFlags = buffer[sizeof(MessageHeader) +
                            sizeof(uint32_t)];  // input flags is the last byte of teh buffer (only 1 byte for now)
}

// Deserialize DisconnectMessage

// ----------------- Deserialize ----------------- //
void server::deserializeGameOverMessage(const std::vector<uint8_t> &buffer, GameOverMessage &msg)
{
    // First deserialize the common message header (4 bytes)
    server::deserializeMessageHeader(buffer, msg.header);

    const size_t expectedSize = sizeof(MessageHeader) + sizeof(uint32_t) + sizeof(uint16_t);
    if (buffer.size() < expectedSize)
    {
        throw std::runtime_error("Buffer too small for GameOverMessage");
    }

    size_t offset = sizeof(MessageHeader);

    // Read clientId and convert from network order
    memcpy(&msg.clientId, buffer.data() + offset, sizeof(uint32_t));
    msg.clientId = ntohl(msg.clientId);
    offset += sizeof(uint32_t);

    // Read condition and convert from network order
    uint16_t rawCondition;
    memcpy(&rawCondition, buffer.data() + offset, sizeof(uint16_t));
    rawCondition = ntohs(rawCondition);
    msg.condition = static_cast<GameOverType>(rawCondition);
    offset += sizeof(uint16_t);
}
