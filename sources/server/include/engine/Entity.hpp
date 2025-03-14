#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <cstddef>

namespace server
{

class Registry;

class Entity
{
  public:
    Entity() : _id(static_cast<size_t>(-1)) {}
    explicit Entity(size_t id) : _id(id) {}

    // Implicit conversion to size_t for component array indexing
    operator size_t() const { return _id; }

    size_t _id;

    // Allow registry to create and manage entities
    friend class Registry;
};

}  // namespace server

#endif  // ENTITY_HPP
