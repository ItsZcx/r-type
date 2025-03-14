#include "game/animation/AnimationManager.hpp"

#include <iostream>
#include <stdexcept>

using namespace client;

void AnimationManager::loadAnimation(const std::string &name, const sf::Texture &texture,
                                     const std::vector<sf::IntRect> &frames)
{
    Animation animation;
    animation.setSpriteSheet(texture);
    std::cout << "Loading animation: " << name << " with " << frames.size() << " frames." << std::endl;
    for (const auto &frame : frames)
    {
        animation.addFrame(frame);
    }
    m_animations[name] = std::move(animation);
}

const Animation &AnimationManager::getAnimation(const std::string &name) const
{
    auto it = m_animations.find(name);
    std::cout << "Retrieving animation: " << name << std::endl;
    if (it == m_animations.end())
    {
        throw std::runtime_error("Animation not found: " + name);
    }
    return it->second;
}
