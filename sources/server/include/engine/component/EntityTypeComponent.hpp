#ifndef ENTITY_TYPE_COMPONENT_HPP
#define ENTITY_TYPE_COMPONENT_HPP

#include <cstdint>

namespace server
{

enum class EntityType : uint16_t
{
    PLAYER = 1,
    MOB = 2,
    BULLET = 3,
    BOSS = 4,
    ORB = 5,
};

struct EntityTypeComponent
{
    EntityType type;
};

}  // namespace server

#endif  // ENTITY_TYPE_COMPONENT_HPP
