/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** type
*/

#ifndef ENTITY_TYPE_HPP_
#define ENTITY_TYPE_HPP_

#include <cstdint>
namespace client
{

enum class EntityType : uint16_t
{
    PLAYER = 1,  // ID 1: Client entity
    MOB = 2,     // ID 2: Mob entity
    BULLET = 3,  // ID 3: Bullet entity
    BOSS = 4,
    ORB = 5
};

enum class OwnerType
{
    PLAYER,
    ENEMY
};

}  // namespace client

#endif /* !ENTITY_TYPE_HPP_ */
