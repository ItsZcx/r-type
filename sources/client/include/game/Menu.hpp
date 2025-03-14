/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Menu
*/

#ifndef MENU_HPP_
#define MENU_HPP_

#include "game/Lobby.hpp"

#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics.hpp>
#include <unordered_map>

namespace client
{

enum mode
{
    MENU,
    GAME_OVER,
    GAME_WIN
};

class Menu
{
  public:
    Menu(sf::RenderWindow &window, NetworkManager &networkManager, Registry &registry);
    ~Menu();

    void run();

    std::string getIp();
    std::string getName();

    void setCreateEntityCallback(CreateEntityCallback createEntityCallback);
    void setMode(mode mode);
    void _stopBackgroundMusic();

  protected:
  private:
    void _loadAssets();
    void _menuPosition();
    void _render();
    void _handleEvents();
    void _handleButtonEvents();
    void _handleTextInputs(const sf::Event &event);
    void _setTexture(sf::Texture &texture, sf::Sprite &sprite, const std::string &path);
    void _setBackgroundTexture(sf::Texture &texture, sf::Sprite &sprite, const std::string &path);
    void _setText(sf::Text &text, sf::Font &font, const sf::Color &color, unsigned int size);
    void _playBackgroundMusic();
    void _setInputPosition(sf::Sprite &box, sf::Text &text);
    void _movingSpritesOut();
    void _adjustSize();

    Lobby _lobby;

    sf::RenderWindow &_window;

    sf::Sprite _playButton;
    sf::Texture _playButtonTexture;

    sf::Sprite _quitButton;
    sf::Texture _quitButtonTexture;

    sf::Sprite _topDesign;
    sf::Texture _topDesignTexture;

    sf::Sprite _bottomDesign;
    sf::Texture _bottomDesignTexture;

    sf::Sprite _background;
    sf::Texture _backgroundTexture;

    sf::Sprite _inputIpBox;
    sf::Sprite _inputNameBox;
    sf::Texture _inputBoxTexture;

    sf::Font _font;
    sf::Text _inputIpText;
    sf::Text _inputNameText;
    std::string _inputIpString;
    std::string _inputNameString;
    sf::Text _ipTitle;
    sf::Text _nameTitle;

    sf::Vector2u _oldWindowSize;
    sf::Vector2u _newWindowSize;

    sf::Music _backgroundMusic;
    sf::Sound _backgroundSound;
    sf::SoundBuffer _backgroundSoundBuffer;

    std::unordered_map<mode, sf::Sprite> _playButtonMap;
    std::unordered_map<mode, sf::Texture> _playButtonTextureMap;

    std::unordered_map<mode, sf::Texture> _titleTextureMap;
    std::unordered_map<mode, sf::Sprite> _titleMap;

    NetworkManager &_networkManager;

    bool _ipSelected;
    bool _nameSelected;

    // std::unordered_map<bool, >

    sf::FloatRect _getRealBounds(sf::FloatRect bounds);
    sf::Vector2f _getRealPosition(sf::Vector2f position);

    mode _mode;

    bool _play;
};

}  // namespace client

#endif /* !MENU_HPP_ */
