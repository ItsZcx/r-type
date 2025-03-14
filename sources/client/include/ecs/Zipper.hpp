/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Zipper
*/

#ifndef ZIPPER_HPP_
#define ZIPPER_HPP_

#include "ZipperIterator.hpp"

namespace client
{

template <typename... Containers> class Zipper
{
  public:
    using iterator = ZipperIterator<Containers...>;
    using iterator_tuple = typename iterator::iterator_tuple;

  public:
    Zipper(Containers &...cs);

    iterator begin();
    iterator end();

  private:
    // helper function to know the maximum index of our iterators.
    static size_t _compute_size(Containers &...containers);
    // helper function to compute an iterator_tuple that will allow us to build our end iterator.
    static iterator_tuple _compute_end(Containers &...containers);

  private:
    iterator_tuple _begin;
    iterator_tuple _end;
    size_t _size;
};

template <typename... Containers>
Zipper<Containers...>::Zipper(Containers &...cs)
    : _begin(std::make_tuple(cs.begin()...)), _end(_compute_end(cs...)), _size(_compute_size(cs...))
{}

/**
 * begin iterator implementation
 * 
 * @tparam Containers the types of the containers
 * @return the iterator that points to the beginning of the Zipper
 */
template <typename... Containers> typename Zipper<Containers...>::iterator Zipper<Containers...>::begin()
{
    return iterator(_begin, _size);
}

/**
 * end iterator implementation
 * 
 * @tparam Containers the types of the containers
 * @return the iterator that points to the end of the Zipper
 */
template <typename... Containers> typename Zipper<Containers...>::iterator Zipper<Containers...>::end()
{
    return iterator(_end, _size);
}

/**
 * Helper function to compute the size of the smallest container
 * 
 * @tparam Containers the types of the containers
 * @param containers the containers
 * @return the size of the Zipper (minimum size of the containers)
 */
template <typename... Containers> size_t Zipper<Containers...>::_compute_size(Containers &...containers)
{
    return std::min({containers.size()...});
}

/**
 * Helper function to compute the end iterator_tuple
 * 
 * @tparam Containers the types of the containers
 * @param containers the containers
 * @return the iterator_tuple that represents the end of each container
 */
template <typename... Containers>
typename Zipper<Containers...>::iterator_tuple Zipper<Containers...>::_compute_end(Containers &...containers)
{
    return std::make_tuple((containers.begin() + _compute_size(containers...))...);
}

}  // namespace client

#endif /* !ZIPPER_HPP_ */
