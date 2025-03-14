#include "EntityUtils.hpp"

#include "Entity.hpp"
#include "Manager.hpp"
#include "PositionComponent.hpp"

#include <cmath>
#include <iostream>
#include <ostream>
#include <utility>
#include <vector>

using namespace server;

VelocityComponent server::calculateOrbVelocity(PositionComponent hitPosition, PositionComponent orbSpawnPoint,
                                               float orbSpeed, int fps)
{
    // Direction vector
    float direction_x = hitPosition.x - orbSpawnPoint.x;
    float direction_y = hitPosition.y - orbSpawnPoint.y;

    // Magnitude of the direction vector
    float magnitude = std::sqrt(direction_x * direction_x + direction_y * direction_y);

    // Normalize the direction vector
    float normalized_x = direction_x / magnitude;
    float normalized_y = direction_y / magnitude;

    // Distance per frame
    float distance_per_frame = orbSpeed / fps;

    // Velocity components
    float velocity_x = normalized_x * distance_per_frame;
    float velocity_y = normalized_y * distance_per_frame;

    return {velocity_x, velocity_y};
}

void server::restartPlayerPositions(Manager &manager)
{
    int i = 0;
    std::vector<Entity> players;
    int playerCount = countPlayers(manager.getRegistry());
    auto clientMap = manager.getClients();

    if (playerCount <= 0)
        return;

    for (const auto &client : clientMap)
    {
        if (manager.hasEntityForClient(client.first))
        {
            Entity player = manager.getEntityForClient(client.first);
            players.push_back(player);
        }
    }

    for (const auto &player : players)
    {
        auto &posArray = manager.getRegistry().get_components<PositionComponent>();
        auto &posOpt = posArray[player];

        if (posOpt.has_value())
        {
            // Place players separated by 100 units in the y-axis + wrap around the world
            posOpt->x = 100.0f;
            posOpt->y = 100.0f + (i * 100.0f);
        }
        i++;
    }
}

int server::countPlayers(Registry &registry)
{
    try
    {
        int count = 0;
        auto entities = registry.get_components<EntityTypeComponent>();

        for (const auto &entity : entities)
        {
            if (entity.has_value() && entity->type == EntityType::PLAYER)
                count++;
        }
        return count;

    } catch (const std::runtime_error &e)
    {
        return -1;
    }
}

void server::killAllEntitiesButPlayers(Registry &registry)
{
    auto entities = registry.get_components<EntityTypeComponent>();

    for (size_t i = 0; i < entities.size(); ++i)
    {
        if (entities[i].has_value() && entities[i]->type != EntityType::PLAYER)
        {
            registry.kill_entity(static_cast<Entity>(i));
        }
    }
}

bool server::mobsAlive(Registry &registry)
{
    bool mobsAlive = false;
    auto aliveEntityTypes = registry.get_components<EntityTypeComponent>();

    for (const auto &type : aliveEntityTypes)
    {
        if (type.has_value() && type->type == EntityType::MOB)
        {
            mobsAlive = true;
            break;
        }
    }

    return mobsAlive;
}

std::chrono::duration<float> getSceneElapsedTime(
    const std::chrono::time_point<std::chrono::high_resolution_clock> &sceneStartTime)
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsedTime = currentTime - sceneStartTime;
    return elapsedTime;
}

int countBullets(Registry &registry)
{
    int bulletCount = 0;
    auto &typeArray = registry.get_components<EntityTypeComponent>();

    for (const auto &type : typeArray)
    {
        if (type.has_value() && type->type == EntityType::BULLET)
        {
            bulletCount++;
        }
    }
    return bulletCount;
}

Entity server::createPlayer(Manager &manager, uint32_t clientId, PositionComponent pos = {100.0f, 100.0f},
                            VelocityComponent vel = {0.0f, 0.0f}, HealthComponent hp = {200})
{
    Registry &registry = manager.getRegistry();
    Entity player = registry.spawn_entity();

    registry.add_component<PositionComponent>(player, std::move(pos));
    registry.add_component<VelocityComponent>(player, std::move(vel));
    registry.add_component<HealthComponent>(player, std::move(hp));
    registry.add_component<EntityTypeComponent>(player, {EntityType::PLAYER});

    manager.mapClientToEntity(clientId, player);
    return player;
}

Entity server::createBoss(Registry &registry, PositionComponent pos, VelocityComponent vel, HealthComponent hp)
{
    Entity boss = registry.spawn_entity();

    registry.add_component<PositionComponent>(boss, std::move(pos));
    registry.add_component<VelocityComponent>(boss, std::move(vel));
    registry.add_component<HealthComponent>(boss, std::move(hp));
    registry.add_component<EntityTypeComponent>(boss, {EntityType::BOSS});

    return boss;
}

Entity server::createMob(Registry &registry, PositionComponent pos = {100.0f, 100.0f},
                         VelocityComponent vel = {0.0f, 0.0f}, HealthComponent hp = {50})
{
    Entity mob = registry.spawn_entity();

    registry.add_component<PositionComponent>(mob, std::move(pos));
    registry.add_component<VelocityComponent>(mob, std::move(vel));
    registry.add_component<HealthComponent>(mob, std::move(hp));
    registry.add_component<EntityTypeComponent>(mob, {EntityType::MOB});

    return mob;
}

Entity server::createBullet(Registry &registry, PositionComponent pos)
{
    Entity bullet = registry.spawn_entity();

    registry.add_component<PositionComponent>(bullet, {pos.x, pos.y + 18.0f});
    registry.add_component<VelocityComponent>(bullet, {10.0f, 0.0f});
    registry.add_component<HealthComponent>(bullet, {1});
    registry.add_component<EntityTypeComponent>(bullet, {EntityType::BULLET});

    return bullet;
}

Entity server::createOrb(Registry &registry, PositionComponent pos, VelocityComponent vel)
{
    Entity orb = registry.spawn_entity();

    registry.add_component<PositionComponent>(orb, {pos.x, pos.y + 18.0f});
    registry.add_component<VelocityComponent>(orb, {vel.vx, vel.vy});
    registry.add_component<HealthComponent>(orb, {1});
    registry.add_component<EntityTypeComponent>(orb, {EntityType::ORB});

    return orb;
}

void server::processUserInput(Manager &manager,
                              const std::chrono::time_point<std::chrono::high_resolution_clock> &sceneStartTime)
{
    UserInputMessage inputMsg;
    static auto lastBulletTime = std::chrono::high_resolution_clock::now();

    while (manager.popInput(inputMsg))
    {
        std::cout << "ECS received input from Client ID: " << inputMsg.clientId
                  << " with flags: " << static_cast<int>(inputMsg.inputFlags) << std::endl;

        if (manager.hasEntityForClient(inputMsg.clientId))
        {
            Entity playerEnt = manager.getEntityForClient(inputMsg.clientId);

            bool moveUp = (inputMsg.inputFlags & static_cast<uint8_t>(InputFlags::MoveUp)) != 0;
            bool moveDown = (inputMsg.inputFlags & static_cast<uint8_t>(InputFlags::MoveDown)) != 0;
            bool moveLeft = (inputMsg.inputFlags & static_cast<uint8_t>(InputFlags::MoveLeft)) != 0;
            bool moveRight = (inputMsg.inputFlags & static_cast<uint8_t>(InputFlags::MoveRight)) != 0;
            bool fire = (inputMsg.inputFlags & static_cast<uint8_t>(InputFlags::Fire)) != 0;

            const float stepSize = 10.0f;
            auto &registry = manager.getRegistry();

            // Directly update position based on inputs (no velocity component)
            auto &posArray = registry.get_components<PositionComponent>();
            auto &posOpt = posArray[playerEnt];  // posOpt is a reference to the optional in the array
            if (posOpt.has_value())
            {
                if (moveUp)
                    posOpt->y -= stepSize;
                if (moveDown)
                    posOpt->y += stepSize;
                if (moveLeft)
                    posOpt->x -= stepSize * 2;
                if (moveRight)
                    posOpt->x += stepSize * 2;
            }

            auto now = std::chrono::high_resolution_clock::now();
            auto timeSinceLastBullet =
                std::chrono::duration_cast<std::chrono::milliseconds>(now - lastBulletTime).count();

            if (fire && timeSinceLastBullet >= 500)  // 500 milliseconds = 0.5 seconds
            {
                auto posOpt = registry.get_components<PositionComponent>()[playerEnt];
                if (posOpt.has_value())
                {
                    createBullet(manager.getRegistry(), posOpt.value());
                    lastBulletTime = now;  // Update the last bullet time
                }
            }
        }
    }
}

StateUpdateMessage server::processOutput(Manager &manager)
{
    StateUpdateMessage stateMsg;
    std::vector<EntityState> entityStates;

    auto &posArray = manager.getRegistry().get_components<PositionComponent>();
    auto &velArray = manager.getRegistry().get_components<VelocityComponent>();
    auto &healthArray = manager.getRegistry().get_components<HealthComponent>();
    auto &typeArray = manager.getRegistry().get_components<EntityTypeComponent>();

    stateMsg.header.messageType = static_cast<uint16_t>(MessageType::StateUpdate);
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
            es.clientId = manager.getClientIdForEntityId(i);
            es.entityId = static_cast<uint32_t>(i);
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

    return stateMsg;
}

bool server::processStartGame(Manager &manager)
{
    UserInputMessage inputMsg;

    while (manager.popInput(inputMsg))
    {
        std::cout << "ECS received input from Client ID: " << inputMsg.clientId
                  << " with flags: " << static_cast<int>(inputMsg.inputFlags) << std::endl;

        if (manager.hasEntityForClient(inputMsg.clientId))
        {
            Entity playerEnt = manager.getEntityForClient(inputMsg.clientId);

            bool startGame = (inputMsg.inputFlags & static_cast<uint8_t>(InputFlags::StartGame)) != 0;
            if (startGame)
            {
                return true;
            }
        }
    }
    return false;
}

void server::processKickPlayer(Manager &manager)
{
    UserInputMessage inputMsg;

    while (manager.popInput(inputMsg))
    {
        std::cout << "ECS received input from Client ID: " << inputMsg.clientId
                  << " with flags: " << static_cast<int>(inputMsg.inputFlags) << std::endl;

        if (manager.hasEntityForClient(inputMsg.clientId))
        {
            Entity playerEnt = manager.getEntityForClient(inputMsg.clientId);

            bool KickPlayer = (inputMsg.inputFlags & static_cast<uint8_t>(InputFlags::KickPlayer)) != 0;
            if (KickPlayer)
            {
                // kick player with id... inputMsg.clientId
                std::cout << "ECS kicking player..." << std::endl;
                // important the id sended here must be the one that you want to kick no the self
                manager.removeClient(inputMsg.clientId);
            }
        }
    }
}
