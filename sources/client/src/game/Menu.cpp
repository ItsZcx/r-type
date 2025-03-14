/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Menu
*/

#include "game/Menu.hpp"

#include <iostream>

using namespace client;

Menu::Menu(sf::RenderWindow &window, NetworkManager &networkManager, Registry &registry)
    : _window(window), _playButtonTexture(), _quitButtonTexture(), _topDesignTexture(), _bottomDesignTexture(),
      _playButton(), _quitButton(), _topDesign(), _bottomDesign(), _play(false), _backgroundTexture(), _background(),
      _ipSelected(false), _nameSelected(false), _lobby(window, networkManager, registry), _mode(MENU),
      _networkManager(networkManager)
{
    _playButtonTextureMap = {
        {MENU,      sf::Texture()},
        {GAME_OVER, sf::Texture()},
        {GAME_WIN,  sf::Texture()}
    };
    _playButtonMap = {
        {MENU,      sf::Sprite()},
        {GAME_OVER, sf::Sprite()},
        {GAME_WIN,  sf::Sprite()}
    };
    _titleTextureMap = {
        {MENU,      sf::Texture()},
        {GAME_OVER, sf::Texture()},
        {GAME_WIN,  sf::Texture()}
    };
    _titleMap = {
        {MENU,      sf::Sprite()},
        {GAME_OVER, sf::Sprite()},
        {GAME_WIN,  sf::Sprite()}
    };
    _loadAssets();
}

Menu::~Menu() {}

void Menu::_loadAssets()
{
    _setBackgroundTexture(_backgroundTexture, _background, "assets/spaceBack.png");

    // _setTexture(_playButtonTexture, _playButton, "assets/menu/play_button.png");

    _setTexture(_playButtonTextureMap[mode::GAME_OVER], _playButtonMap[GAME_OVER], "assets/menu/play_again_button.png");

    _setTexture(_playButtonTextureMap[mode::GAME_WIN], _playButtonMap[GAME_WIN], "assets/menu/play_again_button.png");

    _setTexture(_playButtonTextureMap[mode::MENU], _playButtonMap[MENU], "assets/menu/play_button.png");

    _setTexture(_titleTextureMap[mode::MENU], _titleMap[MENU], "assets/menu/menu_title.png");

    _setTexture(_titleTextureMap[mode::GAME_OVER], _titleMap[GAME_OVER], "assets/menu/game_over_title.png");

    _setTexture(_titleTextureMap[mode::GAME_WIN], _titleMap[GAME_WIN], "assets/menu/win_title.png");

    _setTexture(_quitButtonTexture, _quitButton, "assets/menu/quit_button.png");

    _setTexture(_topDesignTexture, _topDesign, "assets/menu/top_design.png");

    _setTexture(_bottomDesignTexture, _bottomDesign, "assets/menu/bottom_design.png");

    _setTexture(_inputBoxTexture, _inputIpBox, "assets/menu/input_box.png");

    _setTexture(_inputBoxTexture, _inputNameBox, "assets/menu/input_box.png");

    _setText(_inputIpText, _font, sf::Color::White, 35);
    _setText(_inputNameText, _font, sf::Color::White, 35);

    _setText(_ipTitle, _font, sf::Color::White, 30);
    _ipTitle.setString("Enter the IP:");

    _setText(_nameTitle, _font, sf::Color::White, 30);
    _nameTitle.setString("Enter username:");

    _menuPosition();
}

void Menu::_menuPosition()
{
    sf::Vector2u windowSize = _window.getSize();

    float centerX = windowSize.x / 2.0f;
    float centerY = windowSize.y / 2.0f;

    float scaleX = static_cast<float>(windowSize.x) / 1920.0f;
    float scaleY = static_cast<float>(windowSize.y) / 1080.0f;

    sf::FloatRect playButtonBounds = _getRealBounds(_playButtonMap[_mode].getGlobalBounds());
    sf::FloatRect quitButtonBounds = _getRealBounds(_quitButton.getGlobalBounds());
    sf::FloatRect inputIpBoxBounds = _getRealBounds(_inputIpBox.getGlobalBounds());
    sf::FloatRect inputNameBoxBounds = _getRealBounds(_inputNameBox.getGlobalBounds());
    sf::FloatRect ipTitleBounds = _getRealBounds(_ipTitle.getGlobalBounds());
    sf::FloatRect nameTitleBounds = _getRealBounds(_nameTitle.getGlobalBounds());

    _playButtonMap[_mode].setPosition((centerX - playButtonBounds.width / 2.0f) / scaleX,
                                      (centerY - (playButtonBounds.height) * 0.7f) / scaleY);

    _quitButton.setPosition((centerX - quitButtonBounds.width / 2.0f) / scaleX,
                            (centerY + (quitButtonBounds.height * 0.50f)) / scaleY);

    _topDesign.setPosition(0, 0);
    _bottomDesign.setPosition(0, windowSize.y - _getRealBounds(_bottomDesign.getGlobalBounds()).height);

    _titleMap[_mode].setPosition(0, 0);

    _inputIpBox.setPosition((centerX - inputIpBoxBounds.width - 10) / scaleX,
                            (playButtonBounds.top - inputIpBoxBounds.height - 10) / scaleY);

    _inputNameBox.setPosition((centerX + 10) / scaleX,
                              (playButtonBounds.top - inputNameBoxBounds.height - 10) / scaleY);

    _ipTitle.setPosition((_inputIpBox.getPosition().x + inputIpBoxBounds.width / 2.0f) - ipTitleBounds.width / 2.0f,
                         _inputIpBox.getPosition().y - (_ipTitle.getGlobalBounds().height * 1.5f));

    _nameTitle.setPosition((_inputNameBox.getPosition().x + inputNameBoxBounds.width / 2.0f) -
                               nameTitleBounds.width / 2.0f,
                           _inputNameBox.getPosition().y - (_nameTitle.getGlobalBounds().height * 1.5f));

    _setInputPosition(_inputIpBox, _inputIpText);
    _setInputPosition(_inputNameBox, _inputNameText);
}

void Menu::run()
{
    _play = false;
    _playBackgroundMusic();

    while (!_play && _window.isOpen())
    {
        _handleEvents();
        _render();
    }
    std::cout << "Starting game..." << std::endl;
}

void Menu::_handleEvents()
{
    sf::Event event;
    while (_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            _window.close();
            return;
        }
        if (event.type == sf::Event::Resized)
        {
            _adjustSize();
            _menuPosition();
        }
        if (event.type == sf::Event::MouseButtonPressed)
            _handleButtonEvents();
        if (event.type == sf::Event::TextEntered)
        {
            _handleTextInputs(event);
        }
    }
}

void Menu::_handleTextInputs(const sf::Event &event)
{
    std::string *currentString = nullptr;
    sf::Text *currentText = nullptr;
    sf::Sprite *currentBox = nullptr;

    if (_ipSelected)
    {
        currentString = &_inputIpString;
        currentText = &_inputIpText;
        currentBox = &_inputIpBox;
    } else if (_nameSelected)
    {
        currentString = &_inputNameString;
        currentText = &_inputNameText;
        currentBox = &_inputNameBox;
    }

    if (!currentString || !currentText || !currentBox)
        return;

    if (event.text.unicode == '\b')
    {
        if (!currentString->empty())
        {
            currentString->pop_back();
        }
    } else if (event.text.unicode >= 46 && event.text.unicode <= 58 && currentString->size() < 17 && _ipSelected)
    {
        *currentString += static_cast<char>(event.text.unicode);
    } else if (event.text.unicode >= 32 && event.text.unicode <= 126 && currentString->size() < 15 && _nameSelected)
    {
        *currentString += static_cast<char>(event.text.unicode);
    }

    currentText->setString(*currentString);
    _setInputPosition(*currentBox, *currentText);
}

void Menu::_handleButtonEvents()
{
    sf::Vector2u windowSize = _window.getSize();
    sf::Vector2i mousePosition = sf::Mouse::getPosition(_window);

    sf::FloatRect realPlayButtonBounds = _getRealBounds(_playButtonMap[_mode].getGlobalBounds());
    sf::FloatRect realExitButtonBounds = _getRealBounds(_quitButton.getGlobalBounds());

    sf::FloatRect realInputIpBoxBounds = _getRealBounds(_inputIpBox.getGlobalBounds());
    sf::FloatRect realInputNameBoxBounds = _getRealBounds(_inputNameBox.getGlobalBounds());

    if (realPlayButtonBounds.contains(mousePosition.x, mousePosition.y))
    {
        if (_inputIpString.empty() || _inputNameString.empty())
            return;
        // _movingSpritesOut();
        _lobby.setUsername(_inputNameString);
        if (_mode == mode::MENU)
        {
            try
            {
                _networkManager.setIpPort(_inputIpString);
                _networkManager.connectToServer();
            } catch (const std::exception &e)
            {
                std::cerr << e.what() << std::endl;
                return;
            }
        }
        std::cout << "Starting lobby..." << std::endl;
        _lobby.run();
        _play = true;
    } else if (realInputIpBoxBounds.contains(mousePosition.x, mousePosition.y) && _mode == MENU)
    {
        _ipSelected = true;
        _nameSelected = false;
    } else if (realInputNameBoxBounds.contains(mousePosition.x, mousePosition.y) && _mode == MENU)
    {
        _nameSelected = true;
        _ipSelected = false;
    } else if (realExitButtonBounds.contains(mousePosition.x, mousePosition.y))
    {
        _window.close();
    } else
    {
        _ipSelected = false;
        _nameSelected = false;
    }
}

void Menu::_render()
{
    _window.clear(sf::Color::Black);
    _window.draw(_background);
    _window.draw(_topDesign);
    _window.draw(_bottomDesign);
    _window.draw(_titleMap[_mode]);
    _window.draw(_playButtonMap[_mode]);
    _window.draw(_quitButton);
    if (_mode == mode::MENU)
    {
        _window.draw(_inputIpText);
        _window.draw(_inputNameText);
        _window.draw(_inputIpBox);
        _window.draw(_inputNameBox);
        _window.draw(_ipTitle);
        _window.draw(_nameTitle);
    }
    _window.display();
}

void Menu::_setTexture(sf::Texture &texture, sf::Sprite &sprite, const std::string &path)
{
    if (!texture.loadFromFile(path))
        throw std::runtime_error("Failed to load texture.");
    sprite.setTexture(texture);
}

void Menu::_setBackgroundTexture(sf::Texture &texture, sf::Sprite &sprite, const std::string &path)
{
    if (!texture.loadFromFile(path))
        throw std::runtime_error("Failed to load texture.");
    sprite.setTexture(texture);

    float scaleX = static_cast<float>(_window.getSize().x) / texture.getSize().x;
    float scaleY = static_cast<float>(_window.getSize().y) / texture.getSize().y;

    sprite.setScale(scaleX, scaleY);
}

void Menu::_setText(sf::Text &text, sf::Font &font, const sf::Color &color, unsigned int size)
{
    if (!font.loadFromFile("assets/fonts/minecraft.ttf"))
        throw std::runtime_error("Failed to load font.");

    text.setFont(font);
    text.setCharacterSize(size);
    text.setFillColor(color);
}

void Menu::_playBackgroundMusic()
{
    if (!_backgroundSoundBuffer.loadFromFile("assets/A Theme For Space (8bit music).wav"))
    {
        std::cerr << "Failed to load background music." << std::endl;
    }

    _backgroundSound.setBuffer(_backgroundSoundBuffer);
    _backgroundSound.setLoop(true);
    _backgroundSound.setVolume(50);
    _backgroundSound.play();
}

void Menu::_stopBackgroundMusic()
{
    _backgroundSound.stop();
}

void Menu::_movingSpritesOut()
{
    while (1)
    {
        _playButtonMap[_mode].move(-40, 0);
        _quitButton.move(40, 0);
        _topDesign.move(0, -20);
        _bottomDesign.move(0, 20);
        _inputIpBox.move(-40, 0);
        _inputNameBox.move(40, 0);
        _inputIpText.move(-40, 0);
        _inputNameText.move(40, 0);
        _titleMap[_mode].move(0, -20);
        if (_mode == MENU)
        {
            _ipTitle.move(-40, 0);
            _nameTitle.move(40, 0);
        }

        _render();

        if (_playButtonMap[_mode].getPosition().x <= -_playButtonMap[_mode].getGlobalBounds().width)
            break;
    }
}

void Menu::_adjustSize()
{
    sf::Vector2u windowSize = _window.getSize();

    if (windowSize.x < 800)
        windowSize.x = 800;
    if (windowSize.y < 600)
        windowSize.y = 600;

    if (windowSize != _window.getSize())
        _window.setSize(windowSize);
}

void Menu::_setInputPosition(sf::Sprite &box, sf::Text &text)
{
    float inputTextOffsetX = box.getTextureRect().width / 2.0f - text.getGlobalBounds().width / 2.0f;
    float inputTextOffsetY = box.getTextureRect().height / 2.0f - text.getGlobalBounds().height;

    text.setPosition(box.getPosition() + sf::Vector2f(inputTextOffsetX, inputTextOffsetY));
}

sf::FloatRect Menu::_getRealBounds(sf::FloatRect bounds)
{
    sf::Vector2u windowSize = _window.getSize();
    float scaleX = static_cast<float>(windowSize.x) / 1920.0f;
    float scaleY = static_cast<float>(windowSize.y) / 1080.0f;

    return sf::FloatRect(bounds.left * scaleX, bounds.top * scaleY, bounds.width * scaleX, bounds.height * scaleY);
}

std::string Menu::getIp()
{
    return _inputIpString;
}

std::string Menu::getName()
{
    return _inputNameString;
}

void Menu::setCreateEntityCallback(CreateEntityCallback createEntityCallback)
{
    std::cout << "Here" << std::endl;
    _lobby.setCreateEntityCallback(createEntityCallback);
}

void Menu::setMode(mode mode)
{
    _mode = mode;
    _menuPosition();
}
