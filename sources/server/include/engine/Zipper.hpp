#ifndef ZIPPER_HPP
#define ZIPPER_HPP

#include "ZipperIterator.hpp"

namespace server
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

template <typename... Containers> typename Zipper<Containers...>::iterator Zipper<Containers...>::begin()
{
    return ZipperIterator<Containers...>(_begin, _size);
}

template <typename... Containers> typename Zipper<Containers...>::iterator Zipper<Containers...>::end()
{
    return ZipperIterator<Containers...>(_end, _size);
}

template <typename... Containers> size_t Zipper<Containers...>::_compute_size(Containers &...containers)
{
    return std::min({containers.size()...});
}

template <typename... Containers>
typename Zipper<Containers...>::iterator_tuple Zipper<Containers...>::_compute_end(Containers &...containers)
{
    return std::make_tuple((containers.begin() + _compute_size(containers...))...);
}

}  // namespace server

#endif  // ZIPPER_HPP