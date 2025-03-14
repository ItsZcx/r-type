#ifndef VELOCITY_HPP
#define VELOCITY_HPP

#include <ostream>

namespace server
{

struct VelocityComponent
{
    float vx;
    float vy;
};

inline std::ostream &operator<<(std::ostream &os, const VelocityComponent &vel)
{
    return os << "(" << vel.vx << ", " << vel.vy << ")";
}

}  // namespace server

#endif  // VELOCITY_HPP
