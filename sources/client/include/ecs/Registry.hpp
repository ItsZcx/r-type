/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Registry
*/

#ifndef REGISTRY_HPP_
#define REGISTRY_HPP_

#include "Entity.hpp"
#include "SparseArray.hpp"
#include "game/ParallaxLayer.hpp"

#include <SFML/Graphics.hpp>
#include <any>
#include <functional>
#include <iostream>
#include <memory>
#include <typeindex>
#include <unordered_map>

namespace client
{

class Registry
{
  public:
    /**
     * Register a component type with the registry
     * 
     * @tparam Component the type of the component
     * @details If the component type doesn't exist, create a new sparse array
     * and store a removal function for this component type
     * @return the sparse array for the component type
     */
    template <typename Component> SparseArray<Component> &register_component()
    {
        auto typeIndex = std::type_index(typeid(Component));

        if (_components_arrays.find(typeIndex) == _components_arrays.end())
        {
            auto sparseArray = std::make_shared<SparseArray<Component>>();
            _components_arrays[typeIndex] = std::make_shared<std::any>(sparseArray);

            _component_removers[typeIndex] = [this](const Entity &entity) {
                auto &componentArray = get_components<Component>();
                componentArray.erase(static_cast<size_t>(entity));
            };
        }

        return *std::any_cast<std::shared_ptr<SparseArray<Component>>>(*_components_arrays[typeIndex]);
    }

    /**
     * Get the sparse array for a component type
     * 
     * @tparam Component the type of the component
     * @details Call the const version of get_components<Component>() on this object (cast to const Registry*),
     * then use const_cast to remove the constness from the returned reference, allowing modifications.
     * This is a workaround to avoid code duplication, with minimal overhead (only one extra function call)
     * @return the sparse array for the component type
     */
    template <typename Component> SparseArray<Component> &get_components()
    {
        return const_cast<SparseArray<Component> &>(static_cast<const Registry *>(this)->get_components<Component>());
    }

    /**
     * Get the sparse array for a component type
     * 
     * @tparam Component the type of the component
     * @details Find the component type in the map, then cast the stored std::any to a shared pointer
     * If the component type doesn't exist, throw an exception
     * @return the sparse array for the component type
     */
    template <typename Component> Component *get_component(const Entity &entity)
    {
        auto &components = get_components<Component>();
        if (components.size() > static_cast<size_t>(entity) && components[static_cast<size_t>(entity)])
        {
            return &components[static_cast<size_t>(entity)].value();
        }
        return nullptr;
    }

    /**
     * Get the sparse array for a component type
     * 
     * @tparam Component the type of the component
     * @details Find the component type in the map, then cast the stored std::any to a shared pointer
     * If the component type doesn't exist, throw an exception
     * @return the sparse array for the component type
     */
    template <typename Component> const SparseArray<Component> &get_components() const
    {
        auto typeIndex = std::type_index(typeid(Component));

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

        throw std::runtime_error("Component type not registered");
    }

    /**
     * Create a new entity
     * 
     * @param id the ID of the entity
     * @details Find if the entity ID already exists in the entities list
     * If it does, throw an exception
     * Create a new entity with the specified ID and add it to the entities list
     * @return the new entity
     */
    Entity spawn_entity(size_t id)
    {
        Entity entity(id);

        auto check_entity = find_entity(entity);

        if (check_entity)
        {
            throw std::runtime_error("Error: Entity ID already exists!");
        }

        _entities.push_back(entity);

        // std::cout << "Spawned entity with ID: " << id << std::endl;

        return entity;
    }

    /**
     * Remove an entity
     * 
     * @param entity the entity to remove
     * @details Remove components for this entity
     * Remove the entity from the active list
     * Recycle the entity ID
     */
    void kill_entity(const Entity &entity)
    {
        for (auto &remover : _component_removers)
        {
            remover.second(entity);
        }

        auto it = std::find(_entities.begin(), _entities.end(), entity);
        if (it != _entities.end())
        {
            _entities.erase(it);
        }

        _reusable_entity.push_back(entity);
    }

    /**
     * Get the active entities
     * 
     * @return the list of active entities
     */
    const std::vector<Entity> &get_active_entities() const { return _entities; }

    /**
     * Add a component to an entity
     * 
     * @tparam Component the type of the component
     * @param entity the entity to add the component to
     * @param component the component to add
     * @details Call register_component<Component>() to ensure the component type is registered
     * Insert the component into the sparse array for this component type
     * @return the reference to the inserted component
     */
    template <typename Component>
    typename SparseArray<Component>::reference_type add_component(const Entity &entity, Component &&component)
    {
        auto &componentArray = register_component<Component>();
        return componentArray.insert_at(static_cast<size_t>(entity), std::forward<Component>(component));
    }

    template <typename Component>
    typename SparseArray<Component>::reference_type add_component(const Entity &entity, Component &component)
    {
        auto &componentArray = register_component<Component>();
        return componentArray.insert_at(static_cast<size_t>(entity), std::forward<Component>(component));
    }

    /**
     * Emplace a component to an entity
     * 
     * @tparam Component the type of the component
     * @param entity the entity to add the component to
     * @param params the values to construct the component
     * @details The same as add_component, but constructs the component directly in the array,
     * no need to create a temporary object
     * @return the reference to the emplaced component
     */
    template <typename Component, typename... Params>
    typename SparseArray<Component>::reference_type emplace_component(const Entity &entity, Params &&...params)
    {
        auto &componentArray = register_component<Component>();
        return componentArray.emplace_at(static_cast<size_t>(entity), std::forward<Params>(params)...);
    }

    /**
     * Remove a component from an entity
     * 
     * @tparam Component the type of the component
     * @param entity the entity to remove the component from
     * @details Find the removal function for this component type,
     * then call the removal function to erase the component from the array
     */
    template <typename Component> void remove_component(const Entity &entity)
    {
        auto typeIndex = std::type_index(typeid(Component));

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

    /**
     * Add a system to the registry
     * 
     * @tparam Components the types of the components
     * @tparam Function the type of the system function
     * @param func the system function
     * @details Create a lambda to call the system function with the registry and the specified components
     * Add the lambda to the system list
     */
    template <typename... Components, typename Function, typename... Params>
    void add_system(Function const &func, Params &&...params)
    {
        // Calling ex:
        // registry.add_system<PositionComponent, VelocityComponent>(position_system);
        auto system_lambda = [this, &func, &params...]() { func(*this, std::forward<Params>(params)...); };

        _systems.push_back(system_lambda);
    }

    /**
     * Execute all systems
     * 
     * @details Call each system function in the system list
     */
    void run_systems()
    {
        for (auto &system : _systems)
        {
            system();
        }
    }

    bool find_entity(const Entity &entity)
    {
        auto it = std::find(_entities.begin(), _entities.end(), entity);
        return it != _entities.end();
    }

    friend std::ostream &operator<<(std::ostream &os, const Registry &registry);

    void set_health_bar(const sf::RectangleShape &healthBarBox, const sf::RectangleShape &healthBar,
                        const sf::Sprite &heart)
    {
        _healthBarBox = healthBarBox;
        _healthBar = healthBar;
        _heart = heart;
    }

    sf::Sprite &get_heart() { return _heart; }

    sf::RectangleShape &get_health_bar_box() { return _healthBarBox; }

    sf::RectangleShape &get_health_bar() { return _healthBar; }

    void add_parallax_layer(const ParallaxLayer &layer) { _parallaxLayers.push_back(layer); }

    std::vector<ParallaxLayer> &get_parallax_layers() { return _parallaxLayers; }

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
    std::vector<ParallaxLayer> _parallaxLayers;
    sf::RectangleShape _healthBarBox;
    sf::RectangleShape _healthBar;
    sf::Sprite _heart;
};

/**
 * Output stream operator for the Registry
 * 
 * @param os the output stream
 * @param registry the registry to output
 * @details Output the current state of the registry, including live and reusable entity IDs
 * @return the output stream
 */
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

}  // namespace client

#endif /* !REGISTRY_HPP_ */
