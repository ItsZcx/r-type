#include "game/ParallaxLayer.hpp"

#include <iostream>

using namespace client;

ParallaxLayer::ParallaxLayer(const sf::RenderWindow &window, std::shared_ptr<sf::Texture> texture, float speed)
    : _texture(texture), _speed(speed), _offset(0.0f)
{
    _sprite.setTexture(*_texture);
    _texture->setRepeated(true);

    float scaleX = static_cast<float>(window.getSize().x) / _texture->getSize().x;
    float scaleY = static_cast<float>(window.getSize().y) / _texture->getSize().y;
    _sprite.setScale(scaleX, scaleY);

    _sprite.setTextureRect(sf::IntRect(0, 0, window.getSize().x, window.getSize().y));
}

ParallaxLayer::ParallaxLayer(float speed) : _texture(nullptr), _speed(speed), _offset(0.0f)
{
    std::cerr << "Initialized ParallaxLayer for objects only (no background texture)." << std::endl;
}

void ParallaxLayer::addObject(std::shared_ptr<sf::Texture> objectTexture, float x, float y, float speed, float scale)
{
    _objects.emplace_back(objectTexture, x, y, speed, scale);
}

void ParallaxLayer::update(const sf::RenderWindow &window, float deltaTime)
{
    if (_texture)
    {
        _offset += _speed * deltaTime;

        if (_offset > _texture->getSize().x)
        {
            _offset -= _texture->getSize().x;
        }

        _sprite.setTextureRect(sf::IntRect(static_cast<int>(_offset), 0, window.getSize().x, window.getSize().y));
    }

    for (auto &object : _objects)
    {
        object.update(window, deltaTime);
    }
}

void ParallaxLayer::render(sf::RenderWindow &window) const
{
    if (_texture)
    {
        window.draw(_sprite);
    }

    for (const auto &object : _objects)
    {
        window.draw(object.sprite);
    }
}
