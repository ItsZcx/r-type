#ifndef PARALLAX_LAYER_HPP
#define PARALLAX_LAYER_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <memory>
#include <vector>

namespace client
{

class ParallaxLayer
{
  public:
    ParallaxLayer(const sf::RenderWindow &window, std::shared_ptr<sf::Texture> texture, float speed);

    ParallaxLayer(float speed);

    void addObject(std::shared_ptr<sf::Texture> objectTexture, float x, float y, float speed, float scale = 1.0f);
    void update(const sf::RenderWindow &window, float deltaTime);
    void render(sf::RenderWindow &window) const;

  private:
    struct Object
    {
        sf::Sprite sprite;
        std::shared_ptr<sf::Texture> texture;
        float speed;
        float x;
        float y;

        Object(std::shared_ptr<sf::Texture> texture, float x, float y, float speed, float scale)
            : texture(texture), speed(speed), x(x), y(y)
        {
            sprite.setTexture(*texture);
            sprite.setPosition(x, y);
            sprite.setScale(scale, scale);
        }

        void update(const sf::RenderWindow &window, float deltaTime)
        {
            x -= speed * deltaTime;
            if (x + sprite.getGlobalBounds().width < 0)
            {
                x = window.getSize().x;
            }
            sprite.setPosition(x, y);
        }
    };

    sf::Sprite _sprite;
    std::shared_ptr<sf::Texture> _texture;
    float _speed;
    float _offset;
    std::vector<Object> _objects;
};

}  // namespace client

#endif  // PARALLAX_LAYER_HPP
