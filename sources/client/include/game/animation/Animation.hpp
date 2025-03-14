#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <SFML/Graphics.hpp>
#include <vector>

namespace client
{

class Animation
{
  public:
    Animation();

    void addFrame(const sf::IntRect &rect);

    void setSpriteSheet(const sf::Texture &texture);

    const sf::Texture *getSpriteSheet() const;

    void clearFrames();

    std::size_t getSize() const;

    const sf::IntRect &getFrame(std::size_t n) const;

  private:
    std::vector<sf::IntRect> m_frames;
    const sf::Texture *m_texture;
};

}  // namespace client

#endif  // ANIMATION_HPP
