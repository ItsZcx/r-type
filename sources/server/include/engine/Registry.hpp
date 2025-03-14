#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include "Entity.hpp"
#include "SparseArray.hpp"

#include <any>
#include <functional>
#include <iostream>
#include <memory>
#include <typeindex>
#include <unordered_map>

namespace server
{

class Registry
{
  public:
    // Component Registration and Retrieval
    template <typename Component> SparseArray<Component> &register_component()
    {
        auto typeIndex = std::type_index(typeid(Component));

        // If the component type doesn't exist, create a new sparse array
        if (_components_arrays.find(typeIndex) == _components_arrays.end())
        {
            auto sparseArray = std::make_shared<SparseArray<Component>>();
            _components_arrays[typeIndex] = std::make_shared<std::any>(sparseArray);

            // Store a removal function for this component type
            _component_removers[typeIndex] = [this](const Entity &entity) {
                auto &componentArray = get_components<Component>();
                componentArray.erase(static_cast<size_t>(entity));
            };
        }

        // Return the sparse array for this component type
        return *std::any_cast<std::shared_ptr<SparseArray<Component>>>(*_components_arrays[typeIndex]);
    }

    template <typename Component> SparseArray<Component> &get_components()
    {
        // Call the const version of get_components<Component>() on this object (cast to const Registry*),
        // then use const_cast to remove the constness from the returned reference, allowing modifications.
        // This is a workaround to avoid code duplication, with minimal overhead (only one extra function call)
        return const_cast<SparseArray<Component> &>(static_cast<const Registry *>(this)->get_components<Component>());
    }

    template <typename Component> const SparseArray<Component> &get_components() const
    {
        auto typeIndex = std::type_index(typeid(Component));

        // Find and return the sparse array for this component type
        auto it = _components_arrays.find(typeIndex);
        if (it != _components_arrays.end())
        {
            try
            {
                return *std::any_cast<std::shared_ptr<SparseArray<Component>>>(*(it->second));
            } catch (const std::bad_any_cast &e)
            {
                std::cerr << "Type mismatch: expected std::shared_ptr<SparseArray<Component>>, but got "
                          << it->second->type().name() << "\n";
                throw;
            }
        }

        // If no array exists, throw an exception
        throw std::runtime_error("Component type not registered");
    }

    // Entity Management
    Entity spawn_entity()
    {
        size_t id = 0;

        if (!_reusable_entity.empty())
        {
            // Reuse an ID from the pool
            id = _reusable_entity.back();
            _reusable_entity.pop_back();
        } else
        {
            // Use the next available ID
            id = _next_entity++;
        }

        // Add the entity to the active entity list
        Entity entity(id);
        _entities.push_back(entity);
        return entity;
    }

    void kill_entity(const Entity &entity)
    {
        // Remove components for this entity
        for (auto &remover : _component_removers)
        {
            remover.second(entity);
        }

        // Remove the entity from the active list
        auto it = std::find(_entities.begin(), _entities.end(), entity);
        if (it != _entities.end())
        {
            _entities.erase(it);
        }

        // Recycle the entity ID
        _reusable_entity.push_back(entity);
    }

    // Component Management
    template <typename Component>
    typename SparseArray<Component>::reference_type add_component(const Entity &entity, Component &&component)
    {
        auto &componentArray = register_component<Component>();
        return componentArray.insert_at(static_cast<size_t>(entity), std::forward<Component>(component));
    }

    template <typename Component, typename... Params>
    typename SparseArray<Component>::reference_type emplace_component(const Entity &entity, Params &&...params)
    {
        auto &componentArray = register_component<Component>();
        return componentArray.emplace_at(static_cast<size_t>(entity), std::forward<Params>(params)...);
    }

    template <typename Component> void remove_component(const Entity &entity)
    {
        auto typeIndex = std::type_index(typeid(Component));

        // Find the removal function for this component type
        auto it = _component_removers.find(typeIndex);
        if (it != _component_removers.end())
        {
            it->second(entity);
        }
    }

    // template <typename... Components, typename Function>
    // void add_system(Function &&func)
    // {
    //     std::cout << "Called Perfect forwarding in lambda add_system" << std::endl;
    //     auto system_lambda = [this, f = std::forward<Function>(func)]() { f(*this, get_components<Components>()...); };

    //     _systems.push_back(system_lambda);
    // }

    template <typename... Components, typename Function> void add_system(Function const &func)
    {
        // Calling ex:
        // registry.add_system<PositionComponent, VelocityComponent>(position_system);
        auto system_lambda = [this, &func]() { func(*this, get_components<Components>()...); };

        _systems.push_back(system_lambda);
    }

    void run_systems()
    {
        for (auto &system : _systems)
        {
            system();
        }
    }

    void clear_systems() { _systems.clear(); }

    friend std::ostream &operator<<(std::ostream &os, const Registry &registry);

  private:
    // Associative container for component arrays
    std::unordered_map<std::type_index, std::shared_ptr<std::any>> _components_arrays;

    // Container for component removal functions
    std::unordered_map<std::type_index, std::function<void(const Entity &)>> _component_removers;

    // Container for system functions
    std::vector<std::function<void()>> _systems;

    // Entity management
    std::vector<Entity> _entities;
    std::vector<size_t> _reusable_entity;
    size_t _next_entity = 0;
};

inline std::ostream &operator<<(std::ostream &os, const Registry &registry)
{
    os << "Registry:" << std::endl;

    os << " - Live entities id (size_t): [";
    for (const auto &live_entity : registry._entities)
    {
        os << live_entity;
        if (live_entity != registry._entities.back())
        {
            os << ", ";
        }
    }
    os << "]" << std::endl;

    os << " - Reusable entities id (size_t): [";
    for (const auto &dead_entity : registry._reusable_entity)
    {
        os << dead_entity;
        if (dead_entity != registry._reusable_entity.back())
        {
            os << ", ";
        }
    }
    os << "]";

    return os;
}

}  // namespace server

#endif  // REGISTRY_HPP