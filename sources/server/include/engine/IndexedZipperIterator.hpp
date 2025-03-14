#ifndef INDEXED_ZIPPER_ITERATOR_HPP
#define INDEXED_ZIPPER_ITERATOR_HPP

#include <iterator>

namespace server
{

template <typename... Containers> class IndexedZipper;

template <typename... Containers> class IndexedZipperIterator
{
    // type of Container::begin() return value
    template <typename Container> using iterator_t = decltype(std::declval<Container>().begin());

    template <typename Container> using it_reference_t = typename iterator_t<Container>::reference;

  public:
    // std::tuple of references to components
    using value_type = std::tuple<size_t, it_reference_t<Containers>...>;
    using reference = value_type;
    using pointer = void;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;
    using iterator_tuple = std::tuple<iterator_t<Containers>...>;

    // If we want zipper_iterator to be built by zipper only
    friend IndexedZipper<Containers...>;

  public:
    IndexedZipperIterator(IndexedZipperIterator const &z);
    IndexedZipperIterator(iterator_tuple const &it_tuple, size_t max);

    IndexedZipperIterator operator++();
    IndexedZipperIterator &operator++(int);

    value_type operator*();
    value_type operator->();

    template <typename... Cs>
    friend bool operator==(IndexedZipperIterator<Cs...> const &lhs, IndexedZipperIterator<Cs...> const &rhs);

    template <typename... Cs>
    friend bool operator!=(IndexedZipperIterator<Cs...> const &lhs, IndexedZipperIterator<Cs...> const &rhs);

  private:
    // Increment every iterator at the same time. It also skips to the next value if one of the pointed to std::optional does not contains a value
    template <size_t... Is> void incr_all(std::index_sequence<Is...>);
    // check if every std::optional are set
    template <size_t... Is> bool all_set(std::index_sequence<Is...>);
    // return a tuple of reference to components
    template <size_t... Is> value_type to_value(std::index_sequence<Is...>);

  private:
    iterator_tuple _current;
    size_t _max;  // compare this value to _idx to prevent infinite loop
    size_t _idx;  // current index
    static constexpr std::index_sequence_for<Containers...> _seq {};
};

template <typename... Containers>
IndexedZipperIterator<Containers...>::IndexedZipperIterator(IndexedZipperIterator const &z)
    : _current(z._current), _max(z._max), _idx(z._idx)
{
    throw std::logic_error("IndexedZipperIterator is unstable, restrain from using it (out of bounds read)");
}

template <typename... Containers>
IndexedZipperIterator<Containers...>::IndexedZipperIterator(iterator_tuple const &it_tuple, size_t max)
    : _current(it_tuple), _max(max), _idx(0)
{
    if (!all_set(_seq))
    {
        ++(*this);
    }
}

template <typename... Containers>
IndexedZipperIterator<Containers...> IndexedZipperIterator<Containers...>::operator++()
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

template <typename... Containers>
IndexedZipperIterator<Containers...> &IndexedZipperIterator<Containers...>::operator++(int)
{
    IndexedZipperIterator temp = *this;
    ++(*this);
    return temp;
}

template <typename... Containers>
typename IndexedZipperIterator<Containers...>::value_type IndexedZipperIterator<Containers...>::operator*()
{
    return to_value(_seq);
}

template <typename... Containers>
typename IndexedZipperIterator<Containers...>::value_type IndexedZipperIterator<Containers...>::operator->()
{
    return to_value(_seq);
}

template <typename... Containers>
bool operator==(IndexedZipperIterator<Containers...> const &lhs, IndexedZipperIterator<Containers...> const &rhs)
{
    return lhs._idx == rhs._idx;
}

template <typename... Containers>
bool operator!=(IndexedZipperIterator<Containers...> const &lhs, IndexedZipperIterator<Containers...> const &rhs)
{
    return lhs._idx != rhs._idx;
}

template <typename... Containers>
template <size_t... Is>
void IndexedZipperIterator<Containers...>::incr_all(std::index_sequence<Is...>)
{
    (++std::get<Is>(_current), ...);
}

template <typename... Containers>
template <size_t... Is>
bool IndexedZipperIterator<Containers...>::all_set(std::index_sequence<Is...>)
{
    return (std::get<Is>(_current)->has_value() && ...);
}

template <typename... Containers>
template <size_t... Is>
typename IndexedZipperIterator<Containers...>::value_type IndexedZipperIterator<Containers...>::to_value(
    std::index_sequence<Is...>)
{
    // Return a tuple that starts with the current index, followed by component values (tie -> tuple of references)
    return std::tie(_idx, (*std::get<Is>(_current))...);
}

}  // namespace server

#endif  // INDEXED_ZIPPER_ITERATOR_HPP
