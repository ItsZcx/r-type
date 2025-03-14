#include "game/animation/Animation.hpp"

using namespace client;

Animation::Animation() : m_texture(nullptr) {}

void Animation::addFrame(const sf::IntRect &rect)
{
    m_frames.push_back(rect);
}

void Animation::setSpriteSheet(const sf::Texture &texture)
{
    m_texture = &texture;
}

const sf::Texture *Animation::getSpriteSheet() const
{
    return m_texture;
}

void Animation::clearFrames()
{
    m_frames.clear();
}

std::size_t Animation::getSize() const
{
    return m_frames.size();
}

const sf::IntRect &Animation::getFrame(std::size_t n) const
{
    if (n >= m_frames.size())
    {
        throw std::out_of_range("Frame index out of bounds");
    }
    return m_frames[n];
}
