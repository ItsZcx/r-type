/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** position
*/

#ifndef POSITION_HPP_
#define POSITION_HPP_

namespace client
{
namespace components
{
struct position
{
    float x;
    float y;

    position() = default;
    position(float posX, float posY) : x(posX), y(posY) {}
    position(const position &other) = default;
    position &operator=(const position &other) = default;
};

}  // namespace components

}  // namespace client
#endif /* !POSITION_HPP_ */
