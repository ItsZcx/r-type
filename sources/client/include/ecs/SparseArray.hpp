/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** SparseArray
*/

#ifndef SPARSEARRAY_HPP_
#define SPARSEARRAY_HPP_

#include <algorithm>
#include <optional>
#include <vector>

namespace client
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

/**
 * Copy assignment operator
 * 
 * @tparam Component the type of the component 
 * @param other the SparseArray to copy
 * @details If it is not the same as the current SparseArray, copies the internal data vector from the other.
 * @return the copied SparseArray 
 */
template <typename Component> SparseArray<Component> &SparseArray<Component>::operator=(SparseArray const &other)
{
    if (this != &other)
    {
        _data = other._data;
    }
    return *this;
}

/**
 * Move assignment operator
 * 
 * @tparam Component the type of the component
 * @param other the SparseArray to move
 * @details If it is not the same as the other, assign the data of the other
 * @return the moved SparseArray
 */
template <typename Component> SparseArray<Component> &SparseArray<Component>::operator=(SparseArray &&other) noexcept
{
    if (this != &other)
    {
        _data = std::move(other._data);
    }
    return *this;
}

/**
 * Non-const version of the operator[]
 * 
 * @tparam Component the type of the component
 * @param idx the index to access
 * @details Ensures that the vector is resized if the index is out of bounds
 * @return the reference to the component at the specified index
 */
template <typename Component>
typename SparseArray<Component>::reference_type SparseArray<Component>::operator[](size_t idx)
{
    if (idx >= _data.size())
    {
        _data.resize(idx + 1);
    }
    return _data[idx];
}

/**
 * Const version of the operator[]
 * 
 * @tparam Component the type of the component
 * @param idx the index to access
 * @details If the index is out of bounds, returns a static empty optional as a fallback
 * @return the const reference to the component at the specified index
 */
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

/**
 * begin iterator implementation
 * 
 * @tparam Component the type of the component
 * @return the iterator to the beginning of the SparseArray
 */
template <typename Component> typename SparseArray<Component>::iterator SparseArray<Component>::begin()
{
    return _data.begin();
}

/**
 * Const begin iterator implementation
 * 
 * @tparam Component the type of the component
 * @return the const iterator to the beginning of the SparseArray
 */
template <typename Component> typename SparseArray<Component>::const_iterator SparseArray<Component>::begin() const
{
    return _data.begin();
}

/**
 * cbegin iterator implementation
 * 
 * @tparam Component the type of the component
 * @return the const iterator to the beginning of the SparseArray
 */
template <typename Component> typename SparseArray<Component>::const_iterator SparseArray<Component>::cbegin() const
{
    return _data.cbegin();
}

/**
 * end iterator implementation
 * 
 * @tparam Component the type of the component
 * @return the iterator to the end of the SparseArray
 */
template <typename Component> typename SparseArray<Component>::iterator SparseArray<Component>::end()
{
    return _data.end();
}

/**
 * Const end iterator implementation
 * 
 * @tparam Component the type of the component
 * @return the const iterator to the end of the SparseArray
 */
template <typename Component> typename SparseArray<Component>::const_iterator SparseArray<Component>::end() const
{
    return _data.end();
}

/**
 * cend iterator implementation
 * 
 * @tparam Component the type of the component
 * @return the const iterator to the end of the SparseArray
 */
template <typename Component> typename SparseArray<Component>::const_iterator SparseArray<Component>::cend() const
{
    return _data.cend();
}

/**
 * Size of the SparseArray
 * 
 * @tparam Component the type of the component
 * @return the size of the SparseArray
 */
template <typename Component> typename SparseArray<Component>::size_type SparseArray<Component>::size() const
{
    return _data.size();
}

/**
 * Insert a component at the specified position
 * 
 * @tparam Component the type of the component
 * @param pos the position to insert the component
 * @param component the component to insert
 * @details Reuse the SparseArray's operator[] to access the slot at the specified position
 * @return the reference to the inserted component
 */
template <typename Component>
typename SparseArray<Component>::reference_type SparseArray<Component>::insert_at(size_type pos,
                                                                                  const Component &component)
{
    auto &slot = (*this)[pos];
    slot = component;
    return slot;
}

/**
 * Insert a rvalue component at the specified position
 * 
 * @tparam Component the type of the component
 * @param pos the position to insert the component
 * @param component the component to insert
 * @details Reuse the SparseArray's operator[] to access the slot at the specified position
 * @return the reference to the inserted component
 */
template <typename Component>
typename SparseArray<Component>::reference_type SparseArray<Component>::insert_at(size_type pos, Component &&component)
{
    auto &slot = (*this)[pos];
    slot = std::move(component);
    return slot;
}

/**
 * Emplace a component at the specified position
 * 
 * @details First, access the slot at the specified position
 * if the slot already contains a value, reset it before constructing a new one
 * Finally, construct a new object in-place at the specified position
 * using the forwarded parameters
 * 
 * @tparam Component the type of the component
 * @tparam Params the types of the parameters to forward
 * @param pos the position to emplace the component
 * @param params the parameters to forward
 * @return the reference to the emplaced component
 */
template <typename Component>
template <typename... Params>
typename SparseArray<Component>::reference_type SparseArray<Component>::emplace_at(size_type pos, Params &&...params)
{
    auto &slot = (*this)[pos];

    if (slot.has_value())
    {
        slot.reset();
    }

    slot.emplace(std::forward<Params>(params)...);

    return slot;
}

/**
 * Erase a component at the specified position
 * 
 * @tparam Component the type of the component
 * @param pos the position to erase the component
 * @details If the position is within the bounds, resets the value
 * at the specified position without resizing the array
 */
template <typename Component> void SparseArray<Component>::erase(size_type pos)
{
    if (pos < _data.size() && _data[pos].has_value())
    {
        _data[pos].reset();
    }
}

/**
 * Get the index of a component in the SparseArray
 * 
 * @tparam Component the type of the component
 * @param value the value to search for
 * @details If the value is found, returns its index
 * @return the size of the SparseArray to indicate not found
 */
template <typename Component>
typename SparseArray<Component>::size_type SparseArray<Component>::get_index(const value_type &value) const
{
    auto it = std::find(_data.begin(), _data.end(), value);

    if (it != _data.end())
    {
        return static_cast<size_type>(std::distance(_data.begin(), it));
    }

    return _data.size();
}

}  // namespace client

#endif /* !SPARSEARRAY_HPP_ */
