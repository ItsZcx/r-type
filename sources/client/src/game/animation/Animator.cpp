#include "game/animation/Animator.hpp"

using namespace client;

void Animator::addAnimation(const std::string &name, const Animation &animation)
{
    m_animations[name] = animation;
}

void Animator::play(const std::string &name, bool loop)
{
    if (!m_sprite)
    {
        std::cerr << "[ERROR] Cannot play animation without a valid sprite!" << std::endl;
        return;
    }

    if (m_currentAnimationName == name)
    {
        return;
    }

    auto it = m_animations.find(name);
    if (it != m_animations.end())
    {
        m_currentAnimation = &it->second;
        m_currentAnimationName = name;
        m_currentFrame = 0;
        m_elapsedTime = 0.0f;
        m_loop = loop;
        m_finished = false;

        m_sprite->setTexture(*m_currentAnimation->getSpriteSheet());
        m_sprite->setTextureRect(m_currentAnimation->getFrame(m_currentFrame));
    } else
    {
        std::cerr << "[ERROR] Animation '" << name << "' not found!" << std::endl;
    }
}

void Animator::update(float deltaTime)
{
    if (!m_currentAnimation || m_finished || !m_sprite)
    {
        return;
    }

    m_elapsedTime += deltaTime;

    if (m_elapsedTime >= 0.1f)
    {
        m_elapsedTime = 0.0f;
        ++m_currentFrame;

        if (m_currentFrame >= m_currentAnimation->getSize())
        {
            if (m_loop)
            {
                m_currentFrame = 0;
            } else
            {
                m_currentFrame = m_currentAnimation->getSize() - 1;
                m_finished = true;
            }
        }

        if (m_sprite)
        {
            m_sprite->setTextureRect(m_currentAnimation->getFrame(m_currentFrame));
        }
    }
}

bool Animator::isAnimationFinished() const
{
    return m_finished;
}

std::string Animator::getCurrentAnimation() const
{
    std::cout << "Getting current animation" << std::endl;
    if (!m_currentAnimation)
    {
        std::cout << "[ERROR] No current animation!" << std::endl;
        return "";
    }
    std::cout << "Returning current animation" << std::endl;
    return m_currentAnimationName.empty() ? "" : m_currentAnimationName;
}

bool Animator::hasAnimation(const std::string &animationName) const
{
    std::cout << "Checking if animation exists" << std::endl;
    if (m_animations.empty())
    {
        std::cout << "[ERROR] Animator is null!" << std::endl;
        return false;
    }
    std::cout << "Checking" << std::endl;
    if (m_animations.find(animationName) == m_animations.end())
    {
        std::cout << "[ERROR] Animation '" << animationName << "' not found!" << std::endl;
        return false;
    }

    std::cout << "Returning if animation exists" << std::endl;
    return m_animations.find(animationName) != m_animations.end();
}
