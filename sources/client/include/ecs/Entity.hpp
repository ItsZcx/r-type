/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Entity
*/

#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include <cstddef>
#include <limits>

namespace client
{

class Registry;

class Entity
{
  public:
    explicit Entity(size_t id = std::numeric_limits<size_t>::max()) : _id(id) {}

    // Implicit conversion to size_t for component array indexing
    operator size_t() const { return _id; }

    bool isValid() const { return _id != std::numeric_limits<size_t>::max(); }

  private:
    size_t _id;

    // Allow registry to create and manage entities
    friend class Registry;
};

}  // namespace client

#endif /* !ENTITY_HPP_ */
