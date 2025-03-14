#ifndef INDEXED_ZIPPER_HPP
#define INDEXED_ZIPPER_HPP

#include "IndexedZipperIterator.hpp"

#include <algorithm>

namespace server
{

template <typename... Containers> class IndexedZipper
{
  public:
    using iterator = IndexedZipperIterator<Containers...>;
    using iterator_tuple = typename iterator::iterator_tuple;

  public:
    IndexedZipper(Containers &...cs);

    iterator begin();
    iterator end();

  private:
    // Computes maximum index of our iterators.
    static size_t _compute_size(Containers &...containers);
    // Computes an iterator_tuple that will allow us to build our end iterator.
    static iterator_tuple _compute_end(Containers &...containers);

  private:
    iterator_tuple _begin;
    iterator_tuple _end;
    size_t _size;
};

template <typename... Containers>
IndexedZipper<Containers...>::IndexedZipper(Containers &...cs)
    : _begin(std::make_tuple(cs.begin()...)), _end(_compute_end(cs...)), _size(_compute_size(cs...))
{}

template <typename... Containers> typename IndexedZipper<Containers...>::iterator IndexedZipper<Containers...>::begin()
{
    return IndexedZipperIterator<Containers...>(_begin, _size);
}

template <typename... Containers> typename IndexedZipper<Containers...>::iterator IndexedZipper<Containers...>::end()
{
    return IndexedZipperIterator<Containers...>(_end, _size);
}

template <typename... Containers> size_t IndexedZipper<Containers...>::_compute_size(Containers &...containers)
{
    return std::min({containers.size()...});
}

template <typename... Containers>
typename IndexedZipper<Containers...>::iterator_tuple IndexedZipper<Containers...>::_compute_end(
    Containers &...containers)
{
    return std::make_tuple((containers.begin() + _compute_size(containers...))...);
}

}  // namespace server

#endif  // INDEXED_ZIPPER_HPP
