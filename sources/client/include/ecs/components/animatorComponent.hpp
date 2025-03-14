#ifndef ANIMATOR_COMPONENT_HPP_
#define ANIMATOR_COMPONENT_HPP_

#include "game/animation/Animator.hpp"

namespace client
{

namespace components
{

struct AnimatorComponent
{
    Animator animator;

    explicit AnimatorComponent(sf::Sprite *sprite) : animator(sprite)
    {
        std::cout << "AnimatorComponent created with sprite: " << sprite << std::endl;
    }

    AnimatorComponent(AnimatorComponent &&) noexcept = default;
    AnimatorComponent &operator=(AnimatorComponent &&) noexcept = default;

    AnimatorComponent(const AnimatorComponent &) = delete;
    AnimatorComponent &operator=(const AnimatorComponent &) = delete;

    ~AnimatorComponent()
    {
        std::cout << "Destroying AnimatorComponent with sprite: " << animator.get_m_sprite() << std::endl;
    }
};

}  // namespace components

}  // namespace client

#endif  // ANIMATOR_COMPONENT_HPP_
