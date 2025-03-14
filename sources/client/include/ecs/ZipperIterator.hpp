/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ZipperIterator
*/

#ifndef ZIPPERITERATOR_HPP_
#define ZIPPERITERATOR_HPP_

#include <iostream>
#include <iterator>
#include <tuple>

namespace client
{
template <typename... Containers> class Zipper;

template <typename... Containers> class ZipperIterator
{
    // type of Container::begin() return value
    template <typename Container> using iterator_t = decltype(std::declval<Container>().begin());

    template <typename Container> using it_reference_t = typename iterator_t<Container>::reference;

  public:
    // std::tuple of references to components
    using value_type = std::tuple<it_reference_t<Containers>...>;
    using reference = value_type;
    using pointer = void;
    using difference_type = size_t;
    using iterator_category = std::forward_iterator_tag;
    using iterator_tuple = std::tuple<iterator_t<Containers>...>;

    // If we want zipper_iterator to be built by zipper only
    friend Zipper<Containers...>;

  public:
    ZipperIterator(ZipperIterator const &z);
    ZipperIterator(iterator_tuple const &it_tuple, size_t max);

    ZipperIterator operator++();
    ZipperIterator &operator++(int);

    value_type operator*();
    value_type operator->();

    template <typename... Cs>
    friend bool operator==(ZipperIterator<Cs...> const &lhs, ZipperIterator<Cs...> const &rhs);

    template <typename... Cs>
    friend bool operator!=(ZipperIterator<Cs...> const &lhs, ZipperIterator<Cs...> const &rhs);

  private:
    // Increment every iterator at the same time. It also skips to the next value if one of the pointed to std::optional does not contains a value
    template <size_t... Is> void incr_all(std::index_sequence<Is...>);
    // check if every std:: optional are set
    template <size_t... Is> bool all_set(std::index_sequence<Is...>);
    // return a tuple of reference to components
    template <size_t... Is> value_type to_value(std::index_sequence<Is...>);

  private:
    iterator_tuple _current;
    size_t _max;  // compare this value to _idx to prevent infinite loop .
    size_t _idx;
    static constexpr std::index_sequence_for<Containers...> _seq {};
};

template <typename... Containers>
ZipperIterator<Containers...>::ZipperIterator(ZipperIterator const &z)
    : _current(z._current), _max(z._max), _idx(z._idx)
{}

template <typename... Containers>
ZipperIterator<Containers...>::ZipperIterator(iterator_tuple const &it_tuple, size_t max)
    : _current(it_tuple), _max(max), _idx(0)
{
    throw std::logic_error("ZipperIterator is unstable, restrain from using it (out of bounds read)");
    if (!all_set(_seq))
    {
        ++(*this);
    }
}

/**
 * Implementation of the increment operator
 * 
 * Increment _idx and all iterators in _current
 * Ignore invalid positions (where std::optional is empty)
 * 
 * @tparam Containers the types of the containers
 * @return the incremented iterator
 */
template <typename... Containers> ZipperIterator<Containers...> ZipperIterator<Containers...>::operator++()
{
    ++_idx;
    incr_all(_seq);
    while (_idx < _max && !all_set(_seq))
    {
        ++_idx;
        incr_all(_seq);
    }
    return *this;
}

/**
 * Implementation of the post-increment operator
 * 
 * Create a copy of the current iterator and increment the current iterator
 * 
 * @tparam Containers the types of the containers
 * @return the iterator before incrementation
 */
template <typename... Containers> ZipperIterator<Containers...> &ZipperIterator<Containers...>::operator++(int)
{
    ZipperIterator temp = *this;
    ++(*this);
    return temp;
}

/**
 * Dereference operator
 * 
 * @tparam Containers the types of the containers
 * @return the tuple of references to the components at the current position of the containers
 */
template <typename... Containers>
typename ZipperIterator<Containers...>::value_type ZipperIterator<Containers...>::operator*()
{
    return to_value(_seq);
}

/**
 * Arrow operator
 * 
 * @tparam Containers the types of the containers
 * @return the tuple of references to the components at the current position of the containers
 */
template <typename... Containers>
typename ZipperIterator<Containers...>::value_type ZipperIterator<Containers...>::operator->()
{
    return to_value(_seq);
}

/**
 * Equality operator
 * 
 * @tparam Containers the types of the containers
 * @param lhs the left-hand side iterator
 * @param rhs the right-hand side iterator
 * @return true if the iterators are equal, false otherwise
 */
template <typename... Containers>
bool operator==(ZipperIterator<Containers...> const &lhs, ZipperIterator<Containers...> const &rhs)
{
    return lhs._idx == rhs._idx;
}

/**
 * Inequality operator
 * 
 * @tparam Containers the types of the containers
 * @param lhs the left-hand side iterator
 * @param rhs the right-hand side iterator
 * @return true if the iterators are different, false otherwise
 */
template <typename... Containers>
bool operator!=(ZipperIterator<Containers...> const &lhs, ZipperIterator<Containers...> const &rhs)
{
    return lhs._idx != rhs._idx;
}

/**
 * Helper function to increment all iterators at the same time
 * 
 * @tparam Containers the types of the containers
 * @tparam Is the indices of the iterators
 * @param idx_seq the sequence of indices
 */
template <typename... Containers>
template <size_t... Is>
void ZipperIterator<Containers...>::incr_all(std::index_sequence<Is...>)
{
    (++std::get<Is>(_current), ...);
}

/**
 * Helper function to check if every std::optional are set
 * 
 * @tparam Containers the types of the containers
 * @tparam Is the indices of the iterators
 * @param idx_seq the sequence of indices
 * @return true if all std::optional are set, false otherwise
 */
template <typename... Containers>
template <size_t... Is>
bool ZipperIterator<Containers...>::all_set(std::index_sequence<Is...>)
{
    // Unstable read out of bounds with std::optional
    return ((std::get<Is>(_current)->has_value()) && ...);
}

/**
 * Helper function to return a tuple of reference to components
 * 
 * @tparam Containers the types of the containers
 * @tparam Is the indices of the iterators
 * @param idx_seq the sequence of indices
 * @return the tuple of references to the components
 */
template <typename... Containers>
template <size_t... Is>
typename ZipperIterator<Containers...>::value_type ZipperIterator<Containers...>::to_value(std::index_sequence<Is...>)
{
    return std::tie((*std::get<Is>(_current))...);
}

}  // namespace client

#endif /* !ZIPPERITERATOR_HPP_ */
