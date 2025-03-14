#include "Systems.hpp"

#include "AScene.hpp"
#include "EntityTypeComponent.hpp"

#include <cmath>
#include <cstdlib>

using namespace server;

void server::out_of_bounds_system(Registry &r, SparseArray<PositionComponent> &positions,
                                  SparseArray<HealthComponent> &healths, SparseArray<EntityTypeComponent> &types)
{
    for (size_t i = 0; i < positions.size() && i < types.size() && i < healths.size(); ++i)
    {
        if (positions[i].has_value() && types[i].has_value() && healths[i].has_value())
        {
            // Kills bullets that go out of bounds
            if (types[i].value().type == EntityType::BULLET || types[i].value().type == EntityType::ORB)
            {
                if (positions[i].value().x < WORLD_MIN_WIDTH || positions[i].value().x > WORLD_MAX_WIDTH ||
                    positions[i].value().y < WORLD_MIN_HEIGHT || positions[i].value().y > WORLD_MAX_HEIGHT)
                {
                    healths[i].value().value = 0;
                }
            }
            // Makes sure players don't HORZONTALLY wrap around the screen
            if (types[i].value().type == EntityType::PLAYER)
            {
                positions[i].value().x = (positions[i].value().x < WORLD_MIN_WIDTH)         ? WORLD_MIN_WIDTH
                                         : (positions[i].value().x > WORLD_MAX_WIDTH - 175) ? WORLD_MAX_WIDTH - 175
                                                                                            : positions[i].value().x;
            }
        }
    }
}

void server::position_wrapping_system(Registry &r, SparseArray<PositionComponent> &positions,
                                      SparseArray<EntityTypeComponent> &types)
{
    for (size_t i = 0; i < positions.size() && i < types.size(); ++i)
    {
        if (positions[i].has_value() && types[i].has_value())
        {
            if (types[i].value().type == EntityType::BULLET)
                continue;
            // condition ? value_if_true : condition ? value_if_true : value_if_false (AKA default value);
            positions[i].value().y = (positions[i].value().y < WORLD_MIN_HEIGHT)   ? WORLD_MAX_HEIGHT
                                     : (positions[i].value().y > WORLD_MAX_HEIGHT) ? WORLD_MIN_HEIGHT
                                                                                   : positions[i].value().y;

            if (types[i].value().type == EntityType::PLAYER)
                continue;
            positions[i].value().x = (positions[i].value().x < WORLD_MIN_WIDTH - 170) ? WORLD_MAX_WIDTH
                                     : (positions[i].value().x > WORLD_MAX_WIDTH)     ? WORLD_MIN_WIDTH - 170
                                                                                      : positions[i].value().x;
        }
    }
}

void server::position_system(Registry &r, SparseArray<PositionComponent> &positions,
                             SparseArray<VelocityComponent> &velocities)
{
    for (size_t i = 0; i < positions.size() && i < velocities.size(); ++i)
    {
        if (positions[i].has_value() && velocities[i].has_value())
        {
            positions[i].value().x += velocities[i].value().vx;
            positions[i].value().y += velocities[i].value().vy;
        }
    }
}

void server::health_system(Registry &r, SparseArray<HealthComponent> &healths)
{
    for (size_t i = 0; i < healths.size(); ++i)
    {
        if (healths[i].has_value())
        {
            // Kill entity if health <= 0
            if (healths[i].value().value <= 0)
            {
                const Entity &entity = static_cast<Entity>(i);
                r.kill_entity(entity);
                continue;
            }
        }
    }
}

void server::collision_system(Registry &r, SparseArray<PositionComponent> &positions,
                              SparseArray<VelocityComponent> &velocities, SparseArray<HealthComponent> &healths,
                              SparseArray<EntityTypeComponent> &types)
{
    for (size_t curr = 0;
         curr < positions.size() && curr < velocities.size() && curr < healths.size() && curr < types.size(); ++curr)
    {
        if (positions[curr].has_value() && velocities[curr].has_value() && healths[curr].has_value() &&
            types[curr].has_value())
        {
            switch (types[curr].value().type)
            {
                case EntityType::ORB: break;
                case EntityType::MOB: break;
                case EntityType::BOSS: break;
                case EntityType::PLAYER:

                    for (size_t other = 0; other < positions.size() && other < velocities.size() &&
                                           other < healths.size() && other < types.size();
                         ++other)
                    {
                        if (positions[other].has_value() && velocities[other].has_value() &&
                            healths[other].has_value() && types[other].has_value())
                        {
                            // Skip same entity
                            if (curr == other)
                                continue;

                            // Check for player - mob collision
                            if (types[other].value().type == EntityType::MOB)
                            {
                                if (positions[curr].value().x > positions[other].value().x - 100.0f &&
                                    positions[curr].value().x < positions[other].value().x + 200.0f &&
                                    positions[curr].value().y > positions[other].value().y - 40.0f &&
                                    positions[curr].value().y < positions[other].value().y + 10.0f)
                                {
                                    healths[curr].value().value -= 50;
                                }
                            }
                            if (types[other].value().type == EntityType::ORB)
                            {
                                if (positions[curr].value().x + 100.0f > positions[other].value().x &&
                                    positions[curr].value().x < positions[other].value().x &&
                                    positions[curr].value().y + 50.0f > positions[other].value().y &&
                                    positions[curr].value().y < positions[other].value().y)
                                {
                                    healths[curr].value().value -= 20;
                                    healths[other].value().value = 0;
                                }
                            }
                        }
                    }
                case EntityType::BULLET:
                    for (size_t other = 0; other < positions.size() && other < velocities.size() &&
                                           other < healths.size() && other < types.size();
                         ++other)
                    {
                        if (positions[other].has_value() && velocities[other].has_value() &&
                            healths[other].has_value() && types[other].has_value())
                        {
                            if (curr == other)
                                continue;

                            // Check for bullet - mob/boss collision
                            if (types[other].value().type == EntityType::MOB ||
                                types[other].value().type == EntityType::BOSS)
                            {
                                if (positions[curr].value().x > positions[other].value().x - 30.0f &&
                                    positions[curr].value().x < positions[other].value().x + 30.0f &&
                                    positions[curr].value().y > positions[other].value().y - 40.0f &&
                                    positions[curr].value().y < positions[other].value().y + 10.0f)
                                {
                                    healths[other].value().value -= 50;
                                    healths[curr].value().value = 0;
                                }
                            }
                        }
                    }
            }
        }
    }
}