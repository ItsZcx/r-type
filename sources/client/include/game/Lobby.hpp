/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Lobby
*/

#ifndef LOBBY_HPP_
#define LOBBY_HPP_

#include "ecs/Registry.hpp"
#include "network/NetworkManager.hpp"
#include "network/Protocol.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <unordered_map>

namespace client
{

using CreateEntityCallback = std::function<void(const EntityState &)>;

class Lobby
{
  public:
    Lobby(sf::RenderWindow &window, NetworkManager &networkManager, Registry &registry);
    ~Lobby();

    void run();
    void setUsername(const std::string &username);

    void setCreateEntityCallback(CreateEntityCallback createEntityCallback);

  protected:
  private:
    sf::RenderWindow &_window;
    sf::Font _font;
    sf::Event _event;
    CreateEntityCallback _createEntityCallback;

    NetworkManager &_networkManager;
    Registry &_registry;

    std::vector<std::string> _playersUsername;
    std::unordered_map<std::string, bool> _playerStatusMap;

    std::vector<sf::Text> _playersText;

    std::vector<sf::Text> _statusText;

    std::unordered_map<bool, sf::Text> _statusTextMap;

    std::unordered_map<bool, sf::Sprite> _readyButtonMap;
    sf::Texture _readyButtonTexture;
    sf::Texture _notReadyButtonTexture;

    sf::Sprite _topDesign;
    sf::Sprite _bottomDesign;

    sf::Texture _playersTableTexture;
    sf::Sprite _playersTable;

    sf::Texture _lobbyTitleTexture;
    sf::Sprite _lobbyTitle;

    sf::Sprite _background;
    sf::Texture _backgroundTexture;

    bool _play;

    size_t _playerCount;

    void _loadAssets();
    void _render();
    void _handleEvents();
    void _handleButtonEvents();
    void _setTexture(sf::Texture &texture, sf::Sprite &sprite, const std::string &path);
    void _setBackgroundTexture(sf::Texture &texture, sf::Sprite &sprite, const std::string &path);
    void _setText(sf::Text &text, sf::Font &font, const sf::Color &color, unsigned int size);
    void _setMenuPosition();
    void _setButtonPosition(float centerX, float centerY, float scaleX, float scaleY);
    void _movingSpritesOut();
    void _adjustSize();
    void _setPlayersUsernamePosition();
    void _applyStateUpdate(const StateUpdateMessage &stateMsg);
    sf::FloatRect _getRealBounds(const sf::FloatRect bounds);
};

}  // namespace client

#endif /* !LOBBY_HPP_ */
