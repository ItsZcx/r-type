#ifndef SPARSE_ARRAY_HPP
#define SPARSE_ARRAY_HPP

#include "ComponentName.hpp"

#include <algorithm>  // for std::find
#include <optional>
#include <ostream>
#include <vector>

namespace server
{

template <typename Component> class SparseArray
{
  public:
    using value_type = std::optional<Component>;
    using reference_type = value_type &;
    using const_reference_type = value_type const &;
    using container_t = std::vector<value_type>;
    using size_type = typename container_t::size_type;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

  public:
    // Constructors and Destructor
    SparseArray();
    SparseArray(SparseArray const &);      // copy constructor
    SparseArray(SparseArray &&) noexcept;  // move constructor
    ~SparseArray();

    // Assignment Operators
    SparseArray &operator=(SparseArray const &);      // copy assignment operator
    SparseArray &operator=(SparseArray &&) noexcept;  // move assignment operator

    // Element Access
    reference_type operator[](size_t idx);              // non-const version
    const_reference_type operator[](size_t idx) const;  // const version

    // Iterators
    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;

    iterator end();
    const_iterator end() const;
    const_iterator cend() const;

    // Capacity
    size_type size() const;

    // Modifiers
    reference_type insert_at(size_type pos, Component const &);
    reference_type insert_at(size_type pos, Component &&);

    template <typename... Params> reference_type emplace_at(size_type pos, Params &&...);  // optional

    void erase(size_type pos);

    size_type get_index(value_type const &) const;

  private:
    container_t _data;
};

// Default Constructor
template <typename Component> SparseArray<Component>::SparseArray() : _data() {}

// Copy Constructor
template <typename Component> SparseArray<Component>::SparseArray(SparseArray const &other) : _data(other._data) {}

// Move Constructor
template <typename Component> SparseArray<Component>::SparseArray(SparseArray &&other) noexcept
{
    _data = std::move(other._data);
}

// Destructor
template <typename Component> SparseArray<Component>::~SparseArray() {}

// Copy Assignment Operator
template <typename Component> SparseArray<Component> &SparseArray<Component>::operator=(SparseArray const &other)
{
    if (this != &other)
    {
        _data = other._data;
    }
    return *this;
}

// Move Assignment Operator
template <typename Component> SparseArray<Component> &SparseArray<Component>::operator=(SparseArray &&other) noexcept
{
    if (this != &other)
    {
        _data = std::move(other._data);
    }
    return *this;
}

// Element Access Implementations
template <typename Component>
typename SparseArray<Component>::reference_type SparseArray<Component>::operator[](size_t idx)
{
    if (idx >= _data.size())
    {
        _data.resize(idx + 1);
    }
    return _data[idx];
}

template <typename Component>
typename SparseArray<Component>::const_reference_type SparseArray<Component>::operator[](size_t idx) const
{
    static const value_type empty_value {};  // Empty optional for out-of-bounds access

    if (idx >= _data.size())
    {
        return empty_value;
    }
    return _data[idx];
}

// Iterator Implementations
template <typename Component> typename SparseArray<Component>::iterator SparseArray<Component>::begin()
{
    return _data.begin();
}

template <typename Component> typename SparseArray<Component>::const_iterator SparseArray<Component>::begin() const
{
    return _data.begin();
}

template <typename Component> typename SparseArray<Component>::const_iterator SparseArray<Component>::cbegin() const
{
    return _data.cbegin();
}

template <typename Component> typename SparseArray<Component>::iterator SparseArray<Component>::end()
{
    return _data.end();
}

template <typename Component> typename SparseArray<Component>::const_iterator SparseArray<Component>::end() const
{
    return _data.end();
}

template <typename Component> typename SparseArray<Component>::const_iterator SparseArray<Component>::cend() const
{
    return _data.cend();
}

// Capacity Implementation
template <typename Component> typename SparseArray<Component>::size_type SparseArray<Component>::size() const
{
    return _data.size();
}

// Modifier Implementations
template <typename Component>
typename SparseArray<Component>::reference_type SparseArray<Component>::insert_at(size_type pos,
                                                                                  const Component &component)
{
    // Reuse the SparseArray's operator[]
    auto &slot = (*this)[pos];
    slot = component;
    return slot;
}

template <typename Component>
typename SparseArray<Component>::reference_type SparseArray<Component>::insert_at(size_type pos, Component &&component)
{
    // Reuse the SparseArray's operator[]
    auto &slot = (*this)[pos];
    slot = std::move(component);
    return slot;
}

template <typename Component>
template <typename... Params>
typename SparseArray<Component>::reference_type SparseArray<Component>::emplace_at(size_type pos, Params &&...params)
{
    auto &slot = (*this)[pos];

    // Access the allocator used by the vector
    auto &alloc = _data.get_allocator();

    // Destroy the old object before constructing a new one
    if (slot.has_value())
    {
        slot.reset();
    }

    // Construct a new object in-place at the specified position using the forwarded parameters
    std::allocator_traits<typename container_t::allocator_type>::construct(alloc, &_data[pos].value(),
                                                                           std::forward<Params>(params)...);

    return slot;
}

template <typename Component> void SparseArray<Component>::erase(size_type pos)
{
    if (pos < _data.size() && _data[pos].has_value())
    {
        _data[pos].reset();
    }
}

template <typename Component>
typename SparseArray<Component>::size_type SparseArray<Component>::get_index(const value_type &value) const
{
    auto it = std::find(_data.begin(), _data.end(), value);

    if (it != _data.end())
    {
        return static_cast<size_type>(std::distance(_data.begin(), it));
    } else
    {
        return _data.size();  // Return size to indicate not found
    }
}

template <typename Component> std::ostream &operator<<(std::ostream &os, const SparseArray<Component> &sparse_array)
{
    os << "SparseArray " << ComponentName<Component>::get() << ": [";
    for (size_t i = 0; i < sparse_array.size(); ++i)
    {
        if (sparse_array[i].has_value())
        {
            os << *sparse_array[i];
        } else
        {
            os << "nullopt";
        }
        if (i < sparse_array.size() - 1)
        {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

}  // namespace server

#endif  // SPARSE_ARRAY_HPP