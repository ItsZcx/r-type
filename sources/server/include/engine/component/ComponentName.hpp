#ifndef COMPONENT_NAME_HPP
#define COMPONENT_NAME_HPP

#include "EntityTypeComponent.hpp"
#include "HealthComponent.hpp"
#include "PositionComponent.hpp"
#include "VelocityComponent.hpp"

#include <string>
#include <typeinfo>

namespace server
{

////////////////////////////////////////////////////////////////
// Trait so that SparseArray can more easly print information //
////////////////////////////////////////////////////////////////

template <typename T> struct ComponentName
{
    static std::string get() { return typeid(T).name(); }
};

// Specialize the trait for ALL components
template <> struct ComponentName<PositionComponent>
{
    static std::string get() { return "Position"; }
};

template <> struct ComponentName<VelocityComponent>
{
    static std::string get() { return "Velocity"; }
};

template <> struct ComponentName<HealthComponent>
{
    static std::string get() { return "Health"; }
};

template <> struct ComponentName<EntityTypeComponent>
{
    static std::string get() { return "EntityType"; }
};

}  // namespace server

#endif  // COMPONENT_NAME_HPP
