#ifndef POSITION_HPP
#define POSITION_HPP

#include <ostream>

namespace server
{

struct PositionComponent
{
    float x;
    float y;
};

inline std::ostream &operator<<(std::ostream &os, const PositionComponent &pos)
{
    return os << "(" << pos.x << ", " << pos.y << ")";
}

}  // namespace server

#endif  // POSITION_HPP
