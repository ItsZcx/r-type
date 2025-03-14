/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Game
*/

#ifndef GAME_HPP_
#define GAME_HPP_

#include "Menu.hpp"
#include "ecs/Registry.hpp"
#include "ecs/Systems.hpp"
#include "ecs/components/health.hpp"
#include "game/ParallaxLayer.hpp"
#include "game/animation/AnimationManager.hpp"
#include "network/NetworkManager.hpp"

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>
#include <unordered_map>

#define NONE 255

namespace client
{

class Game
{
  public:
    Game();
    ~Game();

    void run();
    void init();

  protected:
    sf::Event _event;

  private:
    sf::RenderWindow _window;
    uint8_t _inputFlags;
    Registry _registry;
    float _deltaTime;
    sf::Clock _clock;
    NetworkManager _networkManager;
    AnimationManager _animationManager;
    Entity _playerEntity;
    Menu _menu;

    std::unordered_map<EntityType, sf::Texture> _textures;
    std::unordered_map<sf::Keyboard::Key, InputFlags> _commands;

    //players textures
    std::unordered_map<size_t, sf::Texture> _playersTextures;

    std::vector<std::string> _colors;

    // variables for updates control
    float _updateInterval;
    float _updateTimer;
    float _timeSinceLastShot;
    const float _shotCooldown = 0.5f;

    uint8_t _processInput(sf::Event event);

    void _createEntity(const EntityState &entityState);
    void _createPlayer(Entity entity, components::position pos, components::velocity vel, components::health health);
    void _createEnemy(Entity entity, components::position pos, components::velocity vel, components::health health);
    void _createBoss(Entity entity, components::position pos, components::velocity vel, components::health health);
    void _createProjectile(Entity entity, components::position pos, components::velocity vel, components::owner owner);
    void _createOrb(Entity entity, components::position pos, components::velocity vel, components::owner owner);

    void _initializeAnimations();
    void _initializeParallax();
    void _setBackground();
    void _setSprite(const Entity entity, components::position pos, EntityType type, float scaleFactor = 1.0f,
                    float scaleX = 200.0, float scaleY = 100.0);
    void _registerComponents();
    void _addSystems();
    void _applyStateUpdate(const StateUpdateMessage &stateMsg);
    void _applyGameOver(const GameOverMessage &gameOverMsg);
    void _applyLocalMovement(uint8_t inputFlags);

    // state update methods
    void _handleEvents();
    void _update();
    void _processStateUpdates();
    void _updateEntity(Entity entity, const EntityState &entityState);

    void _checkHealth();

    template <typename Component> void _updateComponents(const Entity entity, Component newComponent);

    template <typename Component, typename... Params> void _updateComponents(const Entity entity, Params &&...params);

    // Just for testing purposes
    void test();
    bool _isRunning;

    sf::Music _backgroundMusic;
    sf::Sound _backgroundSound;
    sf::SoundBuffer _backgroundSoundBuffer;
    sf::Sound _shotSound;
    sf::SoundBuffer _shotSoundBuffer;

    sf::RectangleShape _healthBarBox;
    sf::RectangleShape _healthBar;
    sf::Texture _heartTexture;

    void _setHealthBar();

    void _playBackgroundMusic();
    void _setShotSound();

    void _networkConnection();

    bool _firstUpdate;

    bool _play;

    std::unordered_map<uint16_t, mode> _modeMap;
};

}  // namespace client

#endif /* !GAME_HPP_ */
