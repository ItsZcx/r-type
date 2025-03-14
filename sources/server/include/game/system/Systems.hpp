#ifndef SYSTEMS_HPP
#define SYSTEMS_HPP

#include "EntityTypeComponent.hpp"
#include "HealthComponent.hpp"
#include "PositionComponent.hpp"
#include "Registry.hpp"
#include "VelocityComponent.hpp"

namespace server
{

void position_system(Registry &r, SparseArray<PositionComponent> &pos, SparseArray<VelocityComponent> &vel);
void position_wrapping_system(Registry &r, SparseArray<PositionComponent> &pos, SparseArray<EntityTypeComponent> &ts);
void out_of_bounds_system(Registry &r, SparseArray<PositionComponent> &pos, SparseArray<HealthComponent> &healths,
                          SparseArray<EntityTypeComponent> &ts);

void health_system(Registry &r, SparseArray<HealthComponent> &healths);
void collision_system(Registry &r, SparseArray<PositionComponent> &pos, SparseArray<VelocityComponent> &vel,
                      SparseArray<HealthComponent> &h, SparseArray<EntityTypeComponent> &ts);

}  // namespace server

#endif  // SYSTEMS_HPP