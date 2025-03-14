#ifndef ANIMATOR_HPP_
#define ANIMATOR_HPP_

#include "Animation.hpp"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <unordered_map>

namespace client
{

class Animator
{
  public:
    explicit Animator(sf::Sprite *sprite) : m_sprite(sprite) {}

    Animator(const Animator &) = delete;
    Animator &operator=(const Animator &) = delete;

    Animator(Animator &&other) noexcept { *this = std::move(other); }

    Animator &operator=(Animator &&other) noexcept
    {
        if (this != &other)
        {
            m_sprite = other.m_sprite;
            m_animations = std::move(other.m_animations);
            m_currentAnimation = other.m_currentAnimation;
            m_currentAnimationName = std::move(other.m_currentAnimationName);
            m_elapsedTime = other.m_elapsedTime;
            m_currentFrame = other.m_currentFrame;
            m_loop = other.m_loop;
            m_finished = other.m_finished;

            other.m_sprite = nullptr;
        }
        return *this;
    }

    ~Animator()
    {
        if (m_sprite)
        {
            std::cout << "Destroying Animator with sprite: " << m_sprite << std::endl;
            m_sprite = nullptr;
        }
    }

    void addAnimation(const std::string &name, const Animation &animation);
    void play(const std::string &name, bool loop = true);
    void update(float deltaTime);
    bool isAnimationFinished() const;
    bool hasAnimation(const std::string &animationName) const;
    std::string getCurrentAnimation() const;
    sf::Sprite *get_m_sprite() const { return m_sprite; }

  private:
    sf::Sprite *m_sprite = nullptr;
    std::unordered_map<std::string, Animation> m_animations;
    const Animation *m_currentAnimation = nullptr;
    std::string m_currentAnimationName;
    float m_elapsedTime = 0.0f;
    std::size_t m_currentFrame = 0;
    bool m_loop = true;
    bool m_finished = false;
};

}  // namespace client

#endif  // ANIMATOR_HPP_
