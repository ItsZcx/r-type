/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Lobby
*/

#include "game/Lobby.hpp"

#include <iostream>

using namespace client;

Lobby::Lobby(sf::RenderWindow &window, NetworkManager &networkManager, Registry &registry)
    : _window(window), _networkManager(networkManager), _registry(registry), _font(), _lobbyTitle(), _playersUsername(),
      _play(false), _playerCount(0)
{
    // std::cout << "Lobby created" << std::endl;

    for (int i = 0; i < 4; i++)
    {
        _playersText.push_back(sf::Text());
        _statusText.push_back(sf::Text());
    }
    // _networkManager.connectToServer();
}

Lobby::~Lobby() {}

void Lobby::run()
{
    setUsername("Player 2");
    setUsername("Player 3");
    setUsername("Player 4");
    _loadAssets();
    _networkManager.setStateUpdateCallback(
        [this](const StateUpdateMessage &stateMsg) { this->_applyStateUpdate(stateMsg); });
    // std::cout << "Running lobby..." << std::endl;
    while (_window.isOpen() && !_play)
    {
        // std::cout << "Running lobby loop..." << std::endl;
        // std::cout << "Before handle events" << std::endl;
        _handleEvents();
        // std::cout << "After handle events" << std::endl;
        _render();
        // std::cout << "After render" << std::endl;
    }
    std::cout << "Starting game..." << std::endl;
}

void Lobby::_loadAssets()
{
    _readyButtonMap[true] = sf::Sprite();
    _readyButtonMap[false] = sf::Sprite();

    // std::cout << "Before setting texture" << std::endl;

    _setBackgroundTexture(_backgroundTexture, _background,
                          "assets/space_background_pack/Assets/Blue Version/blue-preview.png");

    _setTexture(_readyButtonTexture, _readyButtonMap[false], "assets/menu/ready_button.png");

    _setTexture(_notReadyButtonTexture, _readyButtonMap[true], "assets/menu/not_ready_button.png");

    _setTexture(_playersTableTexture, _playersTable, "assets/menu/players_table.png");

    // std::cout << "After setting texture" << std::endl;

    _setText(_statusTextMap[true], _font, sf::Color::Green, 35);
    _statusTextMap[true].setString("Ready");

    _setText(_statusTextMap[false], _font, sf::Color::Red, 35);
    _statusTextMap[false].setString("Not ready");

    _setTexture(_lobbyTitleTexture, _lobbyTitle, "assets/menu/lobby_title.png");

    // std::cout << "After setting text" << std::endl;
    _setMenuPosition();
    // std::cout << "After setting position" << std::endl;
    _setPlayersUsernamePosition();
    // std::cout << "After setting username position" << std::endl;
}

void Lobby::_render()
{
    // std::cout << "Rendering lobby..." << std::endl;
    _window.clear(sf::Color::Black);
    _window.draw(_background);
    // std::cout << "Before Lobby title" << std::endl;
    _window.draw(_lobbyTitle);
    // std::cout << "After Lobby title" << std::endl;
    if (_playersUsername.size() > 0)
        _window.draw(_readyButtonMap[_playerStatusMap[_playersUsername[0]]]);
    // std::cout << "After ready button" << std::endl;
    _window.draw(_topDesign);
    // std::cout << "After top design" << std::endl;
    _window.draw(_bottomDesign);
    // std::cout << "After bottom design" << std::endl;
    _window.draw(_playersTable);
    // std::cout << "After players table" << std::endl;
    // std::cout << "Players in lobby: " << _playersUsername.size() << std::endl;
    for (int i = 0; i < _playerCount; i++)
    {
        _window.draw(_playersText[i]);
        _window.draw(_statusText[i]);
    }
    _window.display();
}

void Lobby::_handleEvents()
{
    while (_window.pollEvent(_event))
    {
        if (_event.type == sf::Event::Closed)
            _window.close();
        if (_event.type == sf::Event::MouseButtonPressed)
            _handleButtonEvents();
        if (_event.type == sf::Event::Resized)
            _adjustSize();
    }
}

void Lobby::_handleButtonEvents()
{
    if (_playersUsername.size() == 0)
        return;
    sf::Vector2i mousePos = sf::Mouse::getPosition(_window);

    bool status = _playerStatusMap[_playersUsername[0]];

    if (_readyButtonMap.find(status) != _readyButtonMap.end())
    {
        sf::FloatRect buttonBounds = _getRealBounds(_readyButtonMap[status].getGlobalBounds());
        if (buttonBounds.contains(mousePos.x, mousePos.y))
        {
            _readyButtonMap[status].setTexture(status ? _notReadyButtonTexture : _readyButtonTexture);
            _playerStatusMap[_playersUsername[0]] = !status;
            _statusText[0].setString(_playerStatusMap[_playersUsername[0]] ? "Ready" : "Not ready");
            _networkManager.sendUserInput(static_cast<uint8_t>(InputFlags::StartGame));
            _play = true;
        }
    }
}

/**
 * Applies state updates received from the server.
 *
 * @param stateMsg: The state update message received.
 */
void Lobby::_applyStateUpdate(const StateUpdateMessage &stateMsg)
{
    for (const auto &entityState : stateMsg.entities)
    {
        Entity entity(static_cast<size_t>(entityState.entityId));

        if (!_registry.find_entity(entity))
        {
            _playerCount++;
            // setUsername("Player " + std::to_string(_playerCount));
            _createEntityCallback(entityState);
        }
    }
}

void Lobby::_setTexture(sf::Texture &texture, sf::Sprite &sprite, const std::string &path)
{
    if (!texture.loadFromFile(path))
        throw std::runtime_error("Failed to load texture.");
    sprite.setTexture(texture);
}

void Lobby::_setBackgroundTexture(sf::Texture &texture, sf::Sprite &sprite, const std::string &path)
{
    if (!texture.loadFromFile(path))
        throw std::runtime_error("Failed to load texture.");
    sprite.setTexture(texture);

    float scaleX = static_cast<float>(1920) / texture.getSize().x;
    float scaleY = static_cast<float>(1080) / texture.getSize().y;

    sprite.setScale(scaleX, scaleY);
}

void Lobby::_setText(sf::Text &text, sf::Font &font, const sf::Color &color, unsigned int size)
{
    if (!font.loadFromFile("assets/fonts/minecraft.ttf"))
        throw std::runtime_error("Failed to load font.");

    text.setFont(font);
    text.setCharacterSize(size);
    text.setFillColor(color);
}

void Lobby::_setMenuPosition()
{
    sf::Vector2u windowSize = _window.getSize();

    float centerX = windowSize.x / 2.0f;
    float centerY = windowSize.y / 2.0f;

    float scaleX = static_cast<float>(windowSize.x) / 1920.0f;
    float scaleY = static_cast<float>(windowSize.y) / 1080.0f;

    sf::FloatRect playersTableBounds = _getRealBounds(_playersTable.getGlobalBounds());

    _playersTable.setPosition((centerX - playersTableBounds.width / 2.0f) / scaleX,
                              (centerY - playersTableBounds.height / 1.2f) / scaleY);

    _setButtonPosition(centerX, centerY, scaleX, scaleY);
}

void Lobby::_setButtonPosition(float centerX, float centerY, float scaleX, float scaleY)
{
    for (auto &button : _readyButtonMap)
    {
        sf::FloatRect buttonBounds = _getRealBounds(button.second.getGlobalBounds());
        button.second.setPosition((centerX - buttonBounds.width / 2.0f) / scaleX,
                                  (centerY + buttonBounds.height) / scaleY);
    }
}

sf::FloatRect Lobby::_getRealBounds(const sf::FloatRect bounds)
{
    sf::Vector2u windowSize = _window.getSize();
    float scaleX = static_cast<float>(windowSize.x) / 1920.0f;
    float scaleY = static_cast<float>(windowSize.y) / 1080.0f;

    return sf::FloatRect(bounds.left * scaleX, bounds.top * scaleY, bounds.width * scaleX, bounds.height * scaleY);
}

void Lobby::setUsername(const std::string &username)
{
    // std::cout << "Setting username: " << username << std::endl;
    _playersUsername.push_back(username);
    _playerStatusMap[username] = false;
    _setPlayersUsernamePosition();
    // std::cout << "Players in lobby: " << _playersUsername.size() << std::endl;
}

void Lobby::_setPlayersUsernamePosition()
{
    if (_playersUsername.size() == 0)
        return;
    sf::Vector2u windowSize = _window.getSize();

    float centerX = windowSize.x / 2.0f;
    float centerY = windowSize.y / 2.0f;

    float scaleX = static_cast<float>(windowSize.x) / 1920.0f;
    float scaleY = static_cast<float>(windowSize.y) / 1080.0f;

    sf::FloatRect tableBounds = _getRealBounds(_playersTable.getGlobalBounds());

    for (int i = 0; i < _playersUsername.size(); i++)
    {
        _setText(_playersText[i], _font, sf::Color::White, 35);
        _setText(_statusText[i], _font, sf::Color::White, 35);

        _playersText[i].setString(_playersUsername[i]);
        _statusText[i].setString(_playerStatusMap[_playersUsername[i]] ? "Ready" : "Not ready");

        if (i == 0)
        {
            sf::FloatRect status = _getRealBounds(_statusText[i].getGlobalBounds());

            _playersText[i].setPosition((centerX - (tableBounds.width / 2.5f)) / scaleX,
                                        (centerY - tableBounds.height / 1.4f) / scaleY);
            _statusText[i].setPosition((centerX + (tableBounds.width / 2.0f) - status.width * 1.2f) / scaleX,
                                       _playersText[i].getPosition().y);
        } else
        {
            sf::FloatRect previousPlayer = _getRealBounds(_playersText[i - 1].getGlobalBounds());
            sf::FloatRect previousStatus = _getRealBounds(_statusText[i - 1].getGlobalBounds());

            _playersText[i].setPosition(previousPlayer.left,
                                        previousPlayer.top + _playersText[i - 1].getGlobalBounds().height + 5);
            _statusText[i].setPosition(previousStatus.left / scaleX, _playersText[i].getPosition().y);
        }
    }
}

void Lobby::_adjustSize()
{
    sf::Vector2u windowSize = _window.getSize();

    if (windowSize.x < 800)
        windowSize.x = 800;
    if (windowSize.y < 600)
        windowSize.y = 600;

    if (windowSize != _window.getSize())
        _window.setSize(windowSize);
}

void Lobby::setCreateEntityCallback(CreateEntityCallback createEntityCallback)
{
    _createEntityCallback = createEntityCallback;
}
