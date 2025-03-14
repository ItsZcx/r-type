#ifndef ANIMATION_MANAGER_HPP_
#define ANIMATION_MANAGER_HPP_

#include "Animation.hpp"

#include <string>
#include <unordered_map>

namespace client
{

class AnimationManager
{
  public:
    void loadAnimation(const std::string &name, const sf::Texture &texture, const std::vector<sf::IntRect> &frames);

    const Animation &getAnimation(const std::string &name) const;

  private:
    std::unordered_map<std::string, Animation> m_animations;
};

}  // namespace client

#endif  // ANIMATION_MANAGER_HPP_
