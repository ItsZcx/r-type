/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Game
*/

#include "game/Game.hpp"

#include "utils/entity_type.hpp"

using namespace client;

/**
 * Constructor of the Game
 * Initializes the window, the input handler, the command, the entities,
 * the entity factory, the renderer and the network manager
 */
Game::Game()
    : _window(sf::VideoMode(1920, 1080, 32), "R-Type", sf::Style::Default), _inputFlags(NONE),
      _networkManager(_deltaTime), _isRunning(true), _registry(), _deltaTime(0.0f), _clock(), _playerEntity(-1),
      _backgroundMusic(), _menu(_window, _networkManager, _registry), _backgroundSoundBuffer(), _backgroundSound(),
      _shotSoundBuffer(), _shotSound(), _firstUpdate(true), _updateTimer(0.0f), _updateInterval(0.0016f),
      _timeSinceLastShot(0.05f), _shotCooldown(0.5f)
{
    _window.setVerticalSyncEnabled(false);
    _window.setFramerateLimit(60);

    _commands = {
        {sf::Keyboard::Up,    InputFlags::MoveUp   },
        {sf::Keyboard::W,     InputFlags::MoveUp   },
        {sf::Keyboard::Down,  InputFlags::MoveDown },
        {sf::Keyboard::S,     InputFlags::MoveDown },
        {sf::Keyboard::Left,  InputFlags::MoveLeft },
        {sf::Keyboard::A,     InputFlags::MoveLeft },
        {sf::Keyboard::Right, InputFlags::MoveRight},
        {sf::Keyboard::D,     InputFlags::MoveRight},
        {sf::Keyboard::Space, InputFlags::Fire     }
    };

    _colors = {"yellow", "blue", "green", "red"};

    _modeMap = {
        {static_cast<uint16_t>(2), mode::GAME_WIN },
        {static_cast<uint16_t>(3), mode::GAME_OVER},
    };

    _addSystems();
    _registerComponents();
    _setShotSound();
    _initializeParallax();
    _initializeAnimations();
    _setHealthBar();

    _menu.setCreateEntityCallback([this](const EntityState &entityState) { this->_createEntity(entityState); });
}

/**
 * @brief Destructor of the Game class.
 * Cleans up resources and disconnects from the server.
 */
Game::~Game()
{
    std::cout << "Destructor Game called. Closing resources..." << std::endl;
    _isRunning = false;
    _networkManager.disconnectFromServer();
    std::cout << "Game resources closed." << std::endl;
}

/**
 * @brief Initializes the game and runs the menu and main game loop.
 */
void Game::init()
{
    _createProjectile(Entity(static_cast<size_t>(1000)), {0.0f, 0.0f}, {0.0f, 0.0f}, {OwnerType::PLAYER});
    _registry.kill_entity(Entity(1000));
    std::cout << "Game initialized." << std::endl;

    while (_window.isOpen())
    {
        _menu.run();
        std::cout << "Connecting to server..." << std::endl;
        _networkConnection();
        std::cout << "Running game..." << std::endl;
        run();
    }
}

/**
 * @brief Main game loop.
 */
void Game::run()
{
    std::cout << "Get IP: " << _menu.getIp() << std::endl;
    _menu._stopBackgroundMusic();
    _playBackgroundMusic();
    while (_window.isOpen() && _isRunning)
    {
        _handleEvents();
        _update();
    }
}

void Game::_networkConnection()
{
    _networkManager.setStateUpdateCallback(
        [this](const StateUpdateMessage &stateMsg) { this->_applyStateUpdate(stateMsg); });
    _networkManager.setGameOverCallback(
        [this](const GameOverMessage &gameOverMsg) { this->_applyGameOver(gameOverMsg); });

    // _networkManager.connectToServer();
}

/**
 * @brief Handles all window events, such as user inputs and window closing.
 */
void Game::_handleEvents()
{
    while (_window.pollEvent(_event))
    {
        if (_event.type == sf::Event::KeyPressed)
        {
            uint8_t input = _processInput(_event);

            if (input != NONE)
            {
                _networkManager.sendUserInput(input);

                if (_playerEntity != -1 && input != static_cast<uint8_t>(InputFlags::Fire))
                {
                    _applyLocalMovement(input);
                }
            }
        }

        if (_event.type == sf::Event::Closed)
        {
            _window.close();
        }
    }
}

/**
 * Updates game logic, processes state updates from the server, and runs ECS systems.
 */
void Game::_update()
{
    // _updateTimer += _deltaTime;
    // if (_updateTimer >= _updateInterval || _firstUpdate){
    //     _networkManager.toUpdate();
    // }
    // std::cout << "Updating game..." << std::endl;
    _deltaTime = _clock.restart().asSeconds();
    _timeSinceLastShot += _deltaTime;

    sf::Clock clock;
    clock.restart();

    auto &parallaxLayers = _registry.get_parallax_layers();
    for (auto &layer : parallaxLayers)
    {
        layer.update(_window, _deltaTime);
    }
    std::cout << "Time to update parallax: " << clock.getElapsedTime().asMilliseconds() << std::endl;
    clock.restart();

    _registry.run_systems();
    std::cout << "Time to run systems: " << clock.getElapsedTime().asMilliseconds() << std::endl;
}

/**
 * Applies local movement based on user input.
 *
 * @param inputFlags: Flags representing the user's input.
 */
void Game::_applyLocalMovement(uint8_t inputFlags)
{
    const float speed = 180.0f;

    components::velocity velocity {0.0f, 0.0f};

    if (inputFlags == static_cast<uint8_t>(InputFlags::MoveUp))
        velocity.y = -speed;
    else if (inputFlags == static_cast<uint8_t>(InputFlags::MoveDown))
        velocity.y = speed;
    else if (inputFlags == static_cast<uint8_t>(InputFlags::MoveLeft))
        velocity.x = -speed;
    else if (inputFlags == static_cast<uint8_t>(InputFlags::MoveRight))
        velocity.x = speed;

    _updateComponents<components::velocity>(_playerEntity, velocity);
}

/**
 * Applies state updates received from the server.
 *
 * @param stateMsg: The state update message received.
 */
void Game::_applyStateUpdate(const StateUpdateMessage &stateMsg)
{
    for (const auto &entityState : stateMsg.entities)
    {
        Entity entity(static_cast<size_t>(entityState.entityId));

        if (!_registry.find_entity(entity))
        {
            _createEntity(entityState);
        } else
        {
            _updateEntity(entity, entityState);
        }
    }

    _updateTimer = 0.0f;
    _firstUpdate = false;
}

/**
 * Applies game over message received from the server.
 * 
 * @param gameOverMsg: The game over message received.
 */
void Game::_applyGameOver(const GameOverMessage &gameOverMsg)
{
    std::cout << "Game over message received!" << std::endl;
    std::cout << "Game over condition: " << static_cast<uint16_t>(gameOverMsg.condition) << std::endl;
    std::cout << "Game over client ID: " << gameOverMsg.clientId << std::endl;
    std::cout << "Game over mode: " << _modeMap[static_cast<uint16_t>(gameOverMsg.condition)] << std::endl;
    if (gameOverMsg.clientId != _networkManager.getClientId() || gameOverMsg.condition == GameOverType::None)
        return;
    _menu.setMode(_modeMap[static_cast<uint16_t>(gameOverMsg.condition)]);
    _backgroundSound.stop();
    _isRunning = false;
}

/**
 * Processes the input event and returns the corresponding command
 *
 * @param event: the input event
 * @return the command corresponding to the input event
 */
uint8_t Game::_processInput(sf::Event event)
{
    if (_commands.find(event.key.code) != _commands.end())
    {
        if (event.key.code == sf::Keyboard::Space)
        {
            if (_timeSinceLastShot >= _shotCooldown)
            {
                _timeSinceLastShot = 0.0f;
                _shotSound.play();
                return static_cast<uint8_t>(_commands[event.key.code]);
            }
        } else
        {
            return static_cast<uint8_t>(_commands[event.key.code]);
        }
    }
    return NONE;  // No hay input válido
}

/**
 * Updates the entities based on the received state update
 * 
 * @param entity: the entity to update
 * @param entityState: the state of the entity
 */
void Game::_updateEntity(Entity entity, const EntityState &entityState)
{
    // std::cout << "Entity: " << entityState.entityId << " Health: " << static_cast<int>(entityState.health) << std::endl;
    if (entityState.health <= 0)
    {
        std::cout << "Killing entity " << entityState.entityId << std::endl;
        _registry.kill_entity(entity);
        return;
    }

    components::position pos {entityState.posX, entityState.posY};
    components::velocity vel {entityState.velX, entityState.velY};

    _updateComponents<components::position>(entity, pos);
    _updateComponents<components::velocity>(entity, vel);
    _updateComponents<components::health>(entity, {entityState.health});
    _updateComponents<components::update>(entity, {true});

    // std::cout << "Updated Entity " << entityState.entityId << " Type: " << entityState.entityType << ", "
    //         << " Position: (" << entityState.posX << ", " << entityState.posY << ")"
    //         << " Velocity: (" << entityState.velX << ", " << entityState.velY << ")"
    //         << " Health: " << static_cast<int>(entityState.health) << std::endl;
}

/**
 * Creates an entity based on the received state update
 * 
 * @param entityState: the state of the entity
 */
void Game::_createEntity(const EntityState &entityState)
{
    Entity entity(static_cast<size_t>(entityState.entityId));

    components::position pos {entityState.posX, entityState.posY};
    components::velocity vel {entityState.velX, entityState.velY};

    switch (static_cast<EntityType>(entityState.entityType))
    {
        case EntityType::PLAYER: {
            if (entityState.clientId == _networkManager.getClientId())
                _playerEntity = entity;

            components::health health {entityState.health};
            _createPlayer(entity, pos, vel, health);
            break;
        }
        case EntityType::MOB: {
            components::health health {entityState.health};
            _createEnemy(entity, pos, vel, health);
            break;
        }
        case EntityType::BULLET: {
            components::owner owner {OwnerType::PLAYER};
            _createProjectile(entity, pos, vel, owner);
            break;
        }
        case EntityType::BOSS: {
            components::health health {entityState.health};
            _createBoss(entity, pos, vel, health);
            break;
        }
        case EntityType::ORB: {
            components::owner owner {OwnerType::ENEMY};
            _createOrb(entity, pos, vel, owner);
            break;
        }
        default:
            std::cerr << "Unknown entity type received: " << static_cast<uint16_t>(entityState.entityType) << std::endl;
            break;
    }
}

/**
 * Creates a player entity
 *
 * @param pos: the position of the player
 * @param vel: the velocity of the player
 */
void Game::_createPlayer(Entity entity, components::position pos, components::velocity vel, components::health health)
{
    Entity player = _registry.spawn_entity(entity);

    components::update update {true};

    _registry.add_component<components::type>(player, {EntityType::PLAYER});
    _registry.add_component<components::position>(player, pos);
    _registry.add_component<components::velocity>(player, vel);
    _registry.add_component<components::health>(player, health);
    _registry.add_component<components::update>(player, update);

    _setSprite(player, pos, EntityType::PLAYER, 4.0f);

    auto drawable = _registry.get_component<components::drawable>(player);
    if (!drawable)
    {
        std::cerr << "Error: Failed to retrieve drawable component!" << std::endl;
        return;
    }

    components::AnimatorComponent animatorComponent(&drawable->sprite);
    // std::cout << "Created AnimatorComponent for entity with sprite: " << &drawable->sprite << std::endl;
    animatorComponent.animator.addAnimation("idle",
                                            _animationManager.getAnimation(_colors[player] + "_spaceship_idle"));
    animatorComponent.animator.addAnimation("move",
                                            _animationManager.getAnimation(_colors[player] + "_spaceship_move"));

    _registry.add_component<components::AnimatorComponent>(player, std::move(animatorComponent));
}

/**
 * Creates an enemy entity
 *
 * @param pos: the position of the enemy
 * @param vel: the velocity of the enemy
 */
void Game::_createEnemy(Entity entity, components::position pos, components::velocity vel, components::health health)
{
    Entity enemy = _registry.spawn_entity(entity);

    _registry.add_component<components::type>(enemy, {EntityType::MOB});
    _registry.add_component<components::position>(enemy, pos);
    _registry.add_component<components::velocity>(enemy, vel);
    _registry.add_component<components::health>(enemy, health);
    _registry.add_component<components::update>(enemy, {true});

    _setSprite(enemy, pos, EntityType::MOB, 2.0f, 150.0, 150.0);

    auto drawable = _registry.get_component<components::drawable>(enemy);
    if (!drawable)
    {
        std::cerr << "Error: Failed to retrieve drawable component!" << std::endl;
        return;
    }

    components::AnimatorComponent animatorComponent(&drawable->sprite);
    // std::cout << "Created AnimatorComponent for entity with sprite: " << &drawable->sprite << std::endl;
    animatorComponent.animator.addAnimation("idle", _animationManager.getAnimation("enemy_idle"));
    animatorComponent.animator.addAnimation("move", _animationManager.getAnimation("enemy_move"));

    _registry.add_component<components::AnimatorComponent>(enemy, std::move(animatorComponent));
}

/**
 * Creates a boss entity
 *
 * @param pos: the position of the boss
 * @param vel: the velocity of the boss
 */
void Game::_createBoss(Entity entity, components::position pos, components::velocity vel, components::health health)
{
    Entity boss = _registry.spawn_entity(entity);

    _registry.add_component<components::type>(boss, {EntityType::BOSS});
    _registry.add_component<components::position>(boss, pos);
    _registry.add_component<components::velocity>(boss, vel);
    _registry.add_component<components::health>(boss, health);
    _registry.add_component<components::update>(boss, {true});

    _setSprite(boss, pos, EntityType::BOSS, 6.0f, 150.0, 250.0);

    auto drawable = _registry.get_component<components::drawable>(boss);
    if (!drawable)
    {
        std::cerr << "Error: Failed to retrieve drawable component!" << std::endl;
        return;
    }

    components::AnimatorComponent animatorComponent(&drawable->sprite);
    // std::cout << "Created AnimatorComponent for entity with sprite: " << &drawable->sprite << std::endl;
    animatorComponent.animator.addAnimation("idle", _animationManager.getAnimation("boss_idle"));

    _registry.add_component<components::AnimatorComponent>(boss, std::move(animatorComponent));
}

/**
 * Creates a projectile entity
 *
 * @param pos: the position of the projectile
 * @param vel: the velocity of the projectile
 * @param owner: the owner of the projectile
 * @param path: the path of the sprite of the projectile
 */
void Game::_createProjectile(Entity entity, components::position pos, components::velocity vel, components::owner owner)
{
    Entity projectile = _registry.spawn_entity(entity);

    _registry.add_component<components::type>(projectile, {EntityType::BULLET});
    _registry.add_component<components::owner>(projectile, owner);
    _registry.add_component<components::position>(projectile, pos);
    _registry.add_component<components::velocity>(projectile, vel);
    _registry.add_component<components::update>(projectile, {true});

    _setSprite(projectile, pos, EntityType::BULLET, 1.0f, 200.0, 200.0);

    auto drawable = _registry.get_component<components::drawable>(projectile);
    if (!drawable)
    {
        std::cerr << "Error: Failed to retrieve drawable component!" << std::endl;
        return;
    }

    components::AnimatorComponent animatorComponent(&drawable->sprite);
    // std::cout << "Created AnimatorComponent for entity with sprite: " << &drawable->sprite << std::endl;
    animatorComponent.animator.addAnimation("fly", _animationManager.getAnimation("bullet_fly"));

    _registry.add_component<components::AnimatorComponent>(projectile, std::move(animatorComponent));
}

void Game::_createOrb(Entity entity, components::position pos, components::velocity vel, components::owner owner)
{
    Entity orb = _registry.spawn_entity(entity);

    _registry.add_component<components::type>(orb, {EntityType::ORB});
    _registry.add_component<components::owner>(orb, owner);
    _registry.add_component<components::position>(orb, pos);
    _registry.add_component<components::velocity>(orb, vel);
    _registry.add_component<components::update>(orb, {true});

    _setSprite(orb, pos, EntityType::ORB, 5.0f, 300.0, 200.0);

    auto drawable = _registry.get_component<components::drawable>(orb);
    if (!drawable)
    {
        std::cerr << "Error: Failed to retrieve drawable component!" << std::endl;
        return;
    }

    components::AnimatorComponent animatorComponent(&drawable->sprite);
    // std::cout << "Created AnimatorComponent for entity with sprite: " << &drawable->sprite << std::endl;
    animatorComponent.animator.addAnimation("fly", _animationManager.getAnimation("orb_fly"));

    _registry.add_component<components::AnimatorComponent>(orb, std::move(animatorComponent));
}

/**
 * Sets the sprite of an entity
 *
 * @param entity: the entity to set the sprite
 * @param path: the path of the sprite
 * @param pos: the position of the sprite
 */
void Game::_setSprite(const Entity entity, components::position pos, EntityType type, float scaleFactor, float scaleX,
                      float scaleY)
{
    if (type != EntityType::PLAYER && _textures.find(type) == _textures.end())
    {
        std::cerr << "Error: No texture found for entity type!" << std::endl;
        return;
    }

    sf::Sprite sprite;
    // std::cout << "Entity: " << entity << " Type: " << int(type) << std::endl;
    // std::cout << "Player Type: " << int(EntityType::PLAYER) << std::endl;
    if (type == EntityType::PLAYER && entity < 4)
    {
        // std::cout << "Entity SET: " << entity << std::endl;
        sprite.setTexture(_playersTextures[2]);
        sprite.setPosition(pos.x, pos.y);
    }
    // else if (type == EntityType::BOSS)
    // {
    //     // std::cout << "BOSS ENTITY SET: " << entity << std::endl;
    //     sprite.setTexture(_textures[type]);
    //     sprite.setPosition(pos.x + 328.5, pos.y + 202.5);
    // }
    else
    {
        // std::cout << "OTHER ENTITY SET: " << entity << std::endl;
        sprite.setTexture(_textures[type]);
        sprite.setPosition(pos.x, pos.y);
    }

    scaleX = scaleFactor * (scaleX / sprite.getGlobalBounds().width);
    scaleY = scaleFactor * (scaleY / sprite.getGlobalBounds().height);
    sprite.setScale(scaleX, scaleY);

    _registry.add_component<components::drawable>(entity, {sprite});
}

/**
 * Registers all required ECS components.
 */
void Game::_registerComponents()
{
    _registry.register_component<components::position>();
    _registry.register_component<components::velocity>();
    _registry.register_component<components::drawable>();
    _registry.register_component<components::type>();
    _registry.register_component<components::owner>();
    _registry.register_component<components::health>();
    _registry.register_component<components::AnimatorComponent>();
    _registry.register_component<components::update>();
}

/**
 * Adds required ECS systems to the registry.
 */
void Game::_addSystems()
{
    _registry.add_system(render_system, _window);
    _registry.add_system(movement_system, _deltaTime);
    _registry.add_system(animation_system, _deltaTime);
    _registry.add_system(animation_event_system, _deltaTime);
    _registry.add_system(collision_system);
    _registry.add_system(life_system, _playerEntity);
}

/**
 * Updates the components of an entity.
 *
 * @tparam Component: The component to update.
 * @param entity: The entity to update.
 * @param newComponent: The new component to set.
 */
template <typename Component> void Game::_updateComponents(const Entity entity, Component newComponent)
{
    _registry.remove_component<Component>(entity);
    _registry.add_component<Component>(entity, newComponent);
}

/**
 * Updates the components of an entity.
 *
 * @tparam Component: The component to update.
 * @tparam Params: The parameters to set the new component.
 * @param entity: The entity to update.
 * @param params: The parameters to set the new component.
 */
template <typename Component, typename... Params> void Game::_updateComponents(const Entity entity, Params &&...params)
{
    _registry.remove_component<Component>(entity);
    _registry.emplace_component<Component>(entity, params...);
}

/**
 * Plays the background music.
 */
void Game::_playBackgroundMusic()
{
    if (!_backgroundSoundBuffer.loadFromFile("assets/Phantasy Star 2 soundtrackRise or Fall.wav"))
    {
        std::cerr << "Failed to load background music." << std::endl;
    }

    _backgroundSound.setBuffer(_backgroundSoundBuffer);
    _backgroundSound.setLoop(true);
    _backgroundSound.setVolume(50);
    _backgroundSound.play();
}

/**
 * Sets the shot sound.
 */
void Game::_setShotSound()
{
    if (!_shotSoundBuffer.loadFromFile("assets/laser-shot.ogg"))
    {
        std::cerr << "Failed to load shot sound." << std::endl;
    }

    _shotSound.setBuffer(_shotSoundBuffer);
    _shotSound.setVolume(30);
}

void Game::_setHealthBar()
{
    sf::Sprite heart;
    if (!_heartTexture.loadFromFile("assets/health/heart.png"))
    {
        std::cerr << "Failed to load heart texture." << std::endl;
    }
    heart.setTexture(_heartTexture);
    heart.setPosition(7, 5);

    _healthBarBox.setSize(sf::Vector2f(200, 20));
    _healthBarBox.setOutlineColor(sf::Color::Black);
    _healthBarBox.setOutlineThickness(5);
    _healthBarBox.setFillColor(sf::Color(128, 128, 128));
    _healthBarBox.setPosition(20, 10);

    _healthBar.setSize(sf::Vector2f(200, 20));
    _healthBar.setFillColor(sf::Color(255, 0, 0));
    _healthBar.setPosition(20, 10);
    _registry.set_health_bar(_healthBarBox, _healthBar, heart);
}

/**
 * Initializes the animations for the entities.
 */
void Game::_initializeAnimations()
{
    for (size_t i = 0; i < 4; i++)
    {
        _playersTextures[i].loadFromFile("assets/" + _colors[i] + "_spaceship.png");

        _animationManager.loadAnimation(_colors[i] + "_spaceship_idle", _playersTextures[i],
                                        {
                                            {80, 80,  80, 80},
                                            {80, 240, 80, 80}
        });

        _animationManager.loadAnimation(_colors[i] + "_spaceship_move", _playersTextures[i],
                                        {
                                            {80, 0,   80, 80},
                                            {80, 160, 80, 80},
        });
    }

    sf::Texture enemyTexture;
    enemyTexture.loadFromFile("assets/spaceSprites/Enemy ship 1.png");
    _textures[EntityType::MOB] = std::move(enemyTexture);

    _animationManager.loadAnimation("enemy_idle", _textures[EntityType::MOB],
                                    {
                                        {0, 52, 32, 13},
                                        {0, 65, 32, 13}
    });

    _animationManager.loadAnimation("enemy_move", _textures[EntityType::MOB],
                                    {
                                        {0, 0,  32, 13},
                                        {0, 13, 32, 13},
                                        {0, 26, 32, 13},
                                        {0, 39, 32, 13}
    });

    sf::Texture bulletTexture;
    bulletTexture.loadFromFile("assets/Main ship weapon - Projectile - Rocket.png");
    _textures[EntityType::BULLET] = std::move(bulletTexture);

    _animationManager.loadAnimation("bullet_fly", _textures[EntityType::BULLET],
                                    {
                                        {0, 0,  32, 32},
                                        {0, 32, 32, 32},
                                        {0, 64, 32, 32}
    });

    sf::Texture orbTexture;
    orbTexture.loadFromFile("assets/All_Fire_Bullet_Pixel_16x16.png");
    _textures[EntityType::ORB] = std::move(orbTexture);
    _animationManager.loadAnimation("orb_fly", _textures[EntityType::ORB],
                                    {
                                        {0,  18, 16, 16},
                                        {16, 18, 16, 16},
                                        {32, 18, 16, 16},
                                        {48, 18, 16, 16},
                                        {64, 18, 16, 16}
    });

    sf::Texture bossTexture;
    bossTexture.loadFromFile("assets/spaceSprites/Enemy ship 4.png");
    _textures[EntityType::BOSS] = std::move(bossTexture);
    _animationManager.loadAnimation("boss_idle", _textures[EntityType::BOSS],
                                    {
                                        {0, 15,  73, 27},
                                        {0, 42,  73, 27},
                                        {0, 69,  73, 27},
                                        {0, 96,  73, 27},
                                        {0, 123, 73, 27},
                                        {0, 152, 73, 27},
                                        {0, 179, 73, 27},
                                        {0, 207, 73, 27},
                                        {0, 234, 73, 27},
                                        {0, 260, 73, 27},
    });
}

void Game::_initializeParallax()
{
    auto backgroundTexture = std::make_shared<sf::Texture>();
    if (!backgroundTexture->loadFromFile(
            "assets/space_background_pack/Assets/Blue Version/layered/blue-with-stars_waifu2x_noise3_scale4x.png"))
    {
        std::cerr << "Failed to load background texture!" << std::endl;
        return;
    }
    _registry.add_parallax_layer(ParallaxLayer(_window, backgroundTexture, 10.0f));

    _registry.add_parallax_layer(ParallaxLayer(5.0f));
    auto bigPlanetTexture = std::make_shared<sf::Texture>();
    if (bigPlanetTexture->loadFromFile("assets/space_background_pack/Assets/Blue Version/layered/prop-planet-big.png"))
    {
        auto &layers = _registry.get_parallax_layers();
        layers[1].addObject(bigPlanetTexture, 500.0f, 600.0f, 60.0f, 6.0f);
        layers[1].addObject(bigPlanetTexture, 1500.0f, 500.0f, 65.0f, 5.0f);
    }

    _registry.add_parallax_layer(ParallaxLayer(8.0f));
    auto smallPlanetTexture = std::make_shared<sf::Texture>();
    if (smallPlanetTexture->loadFromFile(
            "assets/space_background_pack/Assets/Blue Version/layered/prop-planet-small.png"))
    {
        auto &layers = _registry.get_parallax_layers();
        layers[2].addObject(smallPlanetTexture, 300.0f, 100.0f, 40.0f, 3.0f);
        layers[2].addObject(smallPlanetTexture, 1200.0f, 400.0f, 45.0f, 3.5f);
        layers[2].addObject(smallPlanetTexture, 1800.0f, 700.0f, 42.0f, 2.5f);
    }

    _registry.add_parallax_layer(ParallaxLayer(12.0f));
    auto asteroidTexture = std::make_shared<sf::Texture>();
    if (asteroidTexture->loadFromFile("assets/space_background_pack/Assets/Blue Version/layered/asteroid-1.png"))
    {
        auto &layers = _registry.get_parallax_layers();
        layers[3].addObject(asteroidTexture, 400.0f, 150.0f, 100.0f, 1.5f);
        layers[3].addObject(asteroidTexture, 600.0f, 350.0f, 100.0f, 1.5f);
        layers[3].addObject(asteroidTexture, 1000.0f, 350.0f, 40.0f, 1.8f);
        layers[3].addObject(asteroidTexture, 1400.0f, 600.0f, 80.0f, 5.0f);
        layers[3].addObject(asteroidTexture, 1600.0f, 200.0f, 120.0f, 10.0f);
        layers[3].addObject(asteroidTexture, 2000.0f, 800.0f, 100.0f, 1.2f);
    }
}
