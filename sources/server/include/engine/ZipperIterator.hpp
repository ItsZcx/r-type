#ifndef ZIPPER_ITERATOR_HPP
#define ZIPPER_ITERATOR_HPP

#include <iostream>
#include <iterator>

namespace server
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
    // std::cout << "-|- ZipperIterator initialized with max: " << _max << std::endl;
    if (!all_set(_seq))
    {
        // std::cout << "-||- ZipperIterator prev ++" << std::endl;
        // std::cout << "Index = " << this->_idx << std::endl;
        ++(*this);
        // std::cout << "-||- ZipperIterator after ++" << std::endl;
    }
    // std::cout << "-|- ZipperIterator end initialized" << std::endl;
}

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

template <typename... Containers> ZipperIterator<Containers...> &ZipperIterator<Containers...>::operator++(int)
{
    ZipperIterator temp = *this;
    ++(*this);
    return temp;
}

template <typename... Containers>
typename ZipperIterator<Containers...>::value_type ZipperIterator<Containers...>::operator*()
{
    return to_value(_seq);
}

template <typename... Containers>
typename ZipperIterator<Containers...>::value_type ZipperIterator<Containers...>::operator->()
{
    return to_value(_seq);
}

template <typename... Containers>
bool operator==(ZipperIterator<Containers...> const &lhs, ZipperIterator<Containers...> const &rhs)
{
    return lhs._idx == rhs._idx;
}

template <typename... Containers>
bool operator!=(ZipperIterator<Containers...> const &lhs, ZipperIterator<Containers...> const &rhs)
{
    return lhs._idx != rhs._idx;
}

template <typename... Containers>
template <size_t... Is>
void ZipperIterator<Containers...>::incr_all(std::index_sequence<Is...>)
{
    (++std::get<Is>(_current), ...);
}

template <typename... Containers>
template <size_t... Is>
bool ZipperIterator<Containers...>::all_set(std::index_sequence<Is...>)
{
    // Unstable read out of bounds with std::optional
    return ((std::get<Is>(_current)->has_value()) && ...);
}

template <typename... Containers>
template <size_t... Is>
typename ZipperIterator<Containers...>::value_type ZipperIterator<Containers...>::to_value(std::index_sequence<Is...>)
{
    return std::tie((*std::get<Is>(_current))...);
}

}  // namespace server

#endif  // ZIPPER_ITERATOR_HPP
