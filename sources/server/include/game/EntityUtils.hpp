#ifndef ENTITY_UTILS_HPP
#define ENTITY_UTILS_HPP

#include "Entity.hpp"
#include "Manager.hpp"

namespace server
{

int countPlayers(Registry &registry);
bool mobsAlive(Registry &registry);
void killAllEntitiesButPlayers(Registry &registry);
void restartPlayerPositions(Manager &manager);
VelocityComponent calculateOrbVelocity(PositionComponent pos1, PositionComponent pos2, float orbSpeed, int fps);

std::chrono::duration<float> getSceneElapsedTime(
    const std::chrono::time_point<std::chrono::high_resolution_clock> &sceneStartTime);

Entity createBoss(Registry &registry, PositionComponent pos, VelocityComponent vel, HealthComponent hp);
Entity createPlayer(Manager &manager, uint32_t clientId, PositionComponent pos, VelocityComponent vel,
                    HealthComponent hp);
Entity createMob(Registry &registry, PositionComponent pos, VelocityComponent vel, HealthComponent hp);
Entity createBullet(Registry &registry, PositionComponent pos);
Entity createOrb(Registry &registry, PositionComponent pos, VelocityComponent vel);

void processUserInput(Manager &manager,
                      const std::chrono::time_point<std::chrono::high_resolution_clock> &sceneStartTime);
StateUpdateMessage processOutput(Manager &manager);

// LOBBY utils
bool processStartGame(Manager &manager);
void processKickPlayer(Manager &manager);

}  // namespace server

#endif  // ENTITY_UTILS_HPP
