#ifndef R_TYPE_CLIENT_HPP
#define R_TYPE_CLIENT_HPP

#ifdef _WIN32
#define _WIN32_WINNT 0x0601
#endif /* !_WIN32 */

#include <iostream>

#include "Network.hpp"
#include "Registry.hpp"
#include "Position.hpp"
#include "Velocity.hpp"
#include "Texture.hpp"
#include "Scale.hpp"
#include "MovementPattern.hpp"
#include "Destroyable.hpp"
#include "Hitbox.hpp"
#include "Screen.hpp"
#include "Damaging.hpp"
#include "Screen.hpp"
#include "PositionSystem.hpp"
#include "DamageSystem.hpp"
#include "MainMenu.hpp"
#include "Tag.hpp"
#include "Score.hpp"
#include "Sprite.hpp"
#include "Assets.hpp"
#include "Button.hpp"
#include "ButtonSystem.hpp"
#include "ChatBox.hpp"
#include "HealthBar.hpp"

namespace rtype
{
    class Game
    {
        public:
            Game(std::string host, std::string port) : _host(host), _port(port), _updateInterval(1.0f / 60), _assets(rtype::Assets::getInstance())
            {
                _regMenu.register_component<Button>();

                _regEnd.register_component<Button>();

                _reg.register_component<Position>();
                _reg.register_component<Velocity>();
                _reg.register_component<Texture>();
                _reg.register_component<Scale>();
                _reg.register_component<MovementPattern>();
                _reg.register_component<Controllable>();
                _reg.register_component<Destroyable>();
                _reg.register_component<Hitbox>();
                _reg.register_component<Damaging>();
                _reg.register_component<Tag>();
                _reg.register_component<Score>();
                _reg.register_component<Sprite>();
                _reg.register_component<Button>();
                _reg.register_component<ChatBox>();
                _reg.register_component<HealthBar>();
                _reg.register_component<Collider>();
            }

            ~Game() = default;

            void textureSystem()
            {
                for (int i = 0; i < _reg.get_entity_number(); ++i) {
                    if (_reg.entity_has_component<Texture>(Entity(i)))
                        continue;

                    if (_reg.entity_has_component<Tag>(Entity(i))) {
                        auto &tag = _reg.get_component<Tag>(Entity(i));

                        switch (tag._tag) {
                        case TagEnum::PLAYER:
                            _reg.add_component<Texture>(Entity(i), {"player", 66, createdPlayers * 18, 33, 17});
                            _reg.add_component<Scale>(Entity(i), {3, 3});
                            createdPlayers++;
                            break;
                        case TagEnum::ENEMY:
                            _reg.add_component<Texture>(Entity(i), {"enemy", 233, 0, 33, 36});
                            _reg.add_component<Scale>(Entity(i), {3, 3});
                            break;
                        case TagEnum::BULLET:
                            _reg.add_component<Texture>(Entity(i), {"bullet", 185, 0, 25, 25});
                            _reg.add_component<Scale>(Entity(i), {3, 3});
                            break;
                        }
                    }
                }
            }

            void healthBarSystem()
            {
                for (int i = 0; i < _reg.get_entity_number(); ++i) {
                    if (_reg.entity_has_component<HealthBar>(Entity(i))) {
                        auto &healthBar = _reg.get_component<HealthBar>(Entity(i));
                        auto &destroyable = _reg.get_component<Destroyable>(Entity(i));
                        auto &pos = _reg.get_component<Position>(Entity(i));

                        healthBar.update(destroyable._hp, pos._x, pos._y - 20);

                        continue;
                    }

                    if (_reg.entity_has_component<HealthBar>(Entity(i))) continue;
                    if (_reg.entity_has_component<Destroyable>(Entity(i)) == false || _reg.entity_has_component<Tag>(Entity(i)) == false) continue;


                    auto &destroyable = _reg.get_component<Destroyable>(Entity(i));
                    auto &tag = _reg.get_component<Tag>(Entity(i));

                    if (tag._tag == TagEnum::PLAYER) {
                        _reg.add_component<HealthBar>(Entity(i), {90, 20, destroyable._hp, sf::Color::Green});
                    } else if (tag._tag == TagEnum::ENEMY) {
                        _reg.add_component<HealthBar>(Entity(i), {90, 20, destroyable._hp, sf::Color::Red});
                    }
                }
            }

            void updateSprite(net::Client &client)
            {
                textureSystem();
                healthBarSystem();
                posSys.positionSystemClient(_reg);

                for (int i = 0; i < _reg.get_entity_number(); i++) {
                    if (!_reg.entity_has_component<Texture>(Entity(i)))
                        continue;

                    auto &texture = _reg.get_component<Texture>(Entity(i));
                    auto &pos = _reg.get_component<Position>(Entity(i));
                    auto &scale = _reg.get_component<Scale>(Entity(i));

                    if (!_reg.entity_has_component<Sprite>(Entity(i))) {
                        sf::Texture &spriteTexture = _assets.getTexture(_reg.get_component<Texture>(Entity(i))._path);
                        _reg.add_component<Sprite>(Entity(i), {spriteTexture});
                        auto &sprite = _reg.get_component<Sprite>(Entity(i))._sprite;

                        sprite.setPosition(pos._x, pos._y);
                        sprite.setTextureRect(sf::IntRect(texture._left, texture._top, texture._width, texture._height));
                        sprite.setScale(scale._scaleX, scale._scaleY);
                    } else {
                        auto &sprite = _reg.get_component<Sprite>(Entity(i))._sprite;
                        sprite.setPosition(pos._x, pos._y);
                    }
                }
                dmgSys.damageSystem(_reg);
            }

            void drawSprite(sf::RenderWindow &window)
            {
                for (int i = 0; i < _reg.get_entity_number(); i++) {
                    if (!_reg.entity_has_component<Sprite>(Entity(i)))
                        continue;

                    auto &sprite = _reg.get_component<Sprite>(Entity(i));

                    window.draw(sprite._sprite);
                }
            }

            bool checkLoseCondition()
            {
                int count = 0;

                for (int i = 0; i < _reg.get_entity_number(); ++i) {
                    if (_reg.entity_has_component<Tag>(Entity(i)) && _reg.entity_has_component<Destroyable>(Entity(i))) {
                        auto &tag = _reg.get_component<Tag>(Entity(i));
                        auto &destroyable = _reg.get_component<Destroyable>(Entity(i));
                        if (tag._tag == TagEnum::PLAYER && destroyable._hp > 0) {
                            count++;
                        }
                    }
                }
                if (count == 0 && createdPlayers > 0)
                    return true;
                return false;
            }

            void menuScene(sf::RenderWindow &window)
            {
                sf::Event event;
                sf::Time elapsedTime = _clock.getElapsedTime();
                bool mouse_clicked = false;

                if (elapsedTime.asSeconds() >= _updateInterval) {
                    // Event
                    while (window.pollEvent(event)) {
                        if (event.type == sf::Event::Closed)
                            window.close();
                        if (event.type == sf::Event::MouseButtonPressed)
                            if (event.mouseButton.button == sf::Mouse::Left)
                                mouse_clicked = true;
                    }

                    // Update
                    btnSys.run(_regMenu, window, mouse_clicked);

                    // Draw
                    window.clear();

                    for (int i = 0; i < _regMenu.get_entity_number(); ++i) {
                        if (_regMenu.entity_has_component<Button>(Entity(i))) {
                            auto &button = _regMenu.get_component<Button>(Entity(i));
                            button.draw(window);
                        }
                    }

                    window.display();

                    _clock.restart();
                }
            }

            void gameScene(sf::RenderWindow &window, net::Client &client)
            {
                sf::Event event;
                sf::Time elapsedTime = _clock.getElapsedTime();

                if (elapsedTime.asSeconds() >= _updateInterval)
                {
                    // Event
                    while (window.pollEvent(event))
                    {
                        if (event.type == sf::Event::Closed)
                            window.close();
                        if (event.type == sf::Event::KeyReleased)
                        {
                            if (event.key.code == sf::Keyboard::Escape)
                            {
                                for (int i = 0; i < _reg.get_entity_number(); ++i)
                                {
                                    if (_reg.entity_has_component<ChatBox>(Entity(i)) == false) continue;

                                    auto &chatbox = _reg.get_component<ChatBox>(Entity(i));
                                    chatbox._isInputSelected = !chatbox._isInputSelected;
                                }
                            }
                        }
                        if (event.type == sf::Event::TextEntered)
                        {
                            for (int i = 0; i < _reg.get_entity_number(); ++i)
                            {
                                if (_reg.entity_has_component<ChatBox>(Entity(i)) == false) continue;

                                auto &chatbox = _reg.get_component<ChatBox>(Entity(i));

                                if (chatbox._isInputSelected == false) continue;

                                if (event.text.unicode < 128 && event.text.unicode != 36)
                                {
                                    if (event.text.unicode == 13)
                                    {
                                        packet::packetHeader header(packet::TEXT_MESSAGE, sizeof(packet::textMessage));
                                        packet::textMessage textMessage(client.getUuid(), chatbox._inputString);
                                        client.sendPacket(header, textMessage);
                                        chatbox._inputString = "";
                                        chatbox._inputText.setString("");
                                    } else if (event.text.unicode == 8)
                                    {
                                        chatbox.removeChar();
                                    } else {
                                        chatbox.addChar(event.text.unicode);
                                    }
                                }
                            }
                        }
                    }
                    // Update
                    updateSprite(client);

                    // Draw
                    window.clear();

                    drawSprite(window);

                    for (int i = 0; i < _reg.get_entity_number(); ++i)
                    {
                        if (_reg.entity_has_component<ChatBox>(Entity(i)))
                        {
                            auto &chatbox = _reg.get_component<ChatBox>(Entity(i));
                            chatbox.draw(window, _assets.getFont("arial"));
                        }
                        if (_reg.entity_has_component<HealthBar>(Entity(i)))
                        {
                            auto &healthBar = _reg.get_component<HealthBar>(Entity(i));
                            healthBar.draw(window);
                        }
                    }

                    window.display();

                    _clock.restart();


                    if (client.isSocketOpen() == false || checkLoseCondition()) {
                        _scene = 4;
                        client.disconnect();
                    }
                }
            }

            void endScene(sf::RenderWindow &window)
            {
                sf::Event event;
                sf::Time elapsedTime = _clock.getElapsedTime();
                bool mouse_clicked = false;

                if (elapsedTime.asSeconds() >= _updateInterval) {
                    // Event
                    while (window.pollEvent(event)) {
                        if (event.type == sf::Event::Closed)
                            window.close();
                        if (event.type == sf::Event::MouseButtonPressed)
                            if (event.mouseButton.button == sf::Mouse::Left)
                                mouse_clicked = true;
                    }

                    // Update
                    btnSys.run(_regEnd, window, mouse_clicked);

                    // Draw
                    window.clear();

                    for (int i = 0; i < _regEnd.get_entity_number(); ++i) {
                        if (_regEnd.entity_has_component<Button>(Entity(i))) {
                            auto &button = _regEnd.get_component<Button>(Entity(i));
                            button.draw(window);
                        }
                    }

                    window.display();

                    _clock.restart();
                }
            }

            void settingsScene(sf::RenderWindow &window)
            {
                sf::Event event;
                sf::Time elapsedTime = _clock.getElapsedTime();

                if (elapsedTime.asSeconds() >= _updateInterval) {
                    // Event
                    while (window.pollEvent(event)) {
                        if (event.type == sf::Event::Closed)
                            window.close();
                    }

                    // Update

                    // Draw
                    window.clear();



                    window.display();

                    _clock.restart();
                }
            }

            void creditsScene(sf::RenderWindow &window)
            {
                sf::Event event;
                sf::Time elapsedTime = _clock.getElapsedTime();

                if (elapsedTime.asSeconds() >= _updateInterval) {
                    // Event
                    while (window.pollEvent(event)) {
                        if (event.type == sf::Event::Closed)
                            window.close();
                    }

                    // Update

                    // Draw
                    window.clear();



                    window.display();

                    _clock.restart();
                }
            }

            void runGame()
            {
                int width = 1920;
                int height = 1080;
                sf::RenderWindow window(sf::VideoMode(width, height), "R-Type");
                asio::io_context io_context;
                net::Client client(io_context, _host, _port, _reg);

                // Ecs Menu
                Entity buttonPlay = _regMenu.spawn_entity();
                _regMenu.add_component<Button>(buttonPlay,
                    {
                        static_cast<float>(width / 2), static_cast<float>(height / 5 * 1),
                        100.f, 100.f,
                        std::string("Play"),
                        _assets.getFont("arial"),
                        std::function<void()>([&]() { _scene = 1; client.run(); std::cout << "Play" << std::endl; })
                    }
                );
                Entity buttonOptions = _regMenu.spawn_entity();
                _regMenu.add_component<Button>(buttonOptions,
                    {
                        static_cast<float>(width / 2), static_cast<float>(height / 5 * 2),
                        100.f, 100.f,
                        std::string("Options"),
                        _assets.getFont("arial"),
                        std::function<void()>([&]() { _scene = 2; std::cout << "Options" << std::endl; })
                    }
                );
                Entity buttonCredits = _regMenu.spawn_entity();
                _regMenu.add_component<Button>(buttonCredits,
                    {
                        static_cast<float>(width / 2), static_cast<float>(height / 5 * 3),
                        100.f, 100.f,
                        std::string("Credits"),
                        _assets.getFont("arial"),
                        std::function<void()>([&]() { _scene = 3; std::cout << "Credits" << std::endl; })
                    }
                );
                Entity buttonQuit = _regMenu.spawn_entity();
                _regMenu.add_component<Button>(buttonQuit,
                    {
                        static_cast<float>(width / 2), static_cast<float>(height / 5 * 4),
                        100.f, 100.f,
                        std::string("Quit"),
                        _assets.getFont("arial"),
                        std::function<void()>([&]() { window.close(); std::cout << "Quit" << std::endl; })
                    }
                );

                // Ecs End
                Entity buttonEndMenu = _regEnd.spawn_entity();
                _regEnd.add_component<Button>(buttonEndMenu,
                    {
                        static_cast<float>(width / 2), static_cast<float>(height / 5 * 1),
                        100.f, 100.f,
                        std::string("Menu"),
                        _assets.getFont("arial"),
                        std::function<void()>([&]() { _scene = 0; std::cout << "Menu" << std::endl; })
                    }
                );
                Entity buttonEndQuit = _regEnd.spawn_entity();
                _regEnd.add_component<Button>(buttonEndQuit,
                    {
                        static_cast<float>(width / 2), static_cast<float>(height / 5 * 2),
                        100.f, 100.f,
                        std::string("Quit"),
                        _assets.getFont("arial"),
                        std::function<void()>([&]() { window.close(); std::cout << "Quit" << std::endl; })
                    }
                );


                // Ecs Game
                Entity Space_background = _reg.spawn_entity();
                _reg.add_component<Texture>(Space_background, {"space", 0, 0, 950, 200});
                _reg.add_component<Position>(Space_background, {0, 0});
                _reg.add_component<Scale>(Space_background, {5, 5});
                _reg.add_component<Velocity>(Space_background, {1});
                _reg.add_component<MovementPattern>(Space_background, {STRAIGHTLEFT});

                Entity chatbox = _reg.spawn_entity();
                _reg.add_component<ChatBox>(chatbox, {0, 0, 600, 200 });

                mainMenu mainMenu(window.getSize().x, window.getSize().y);

                while (window.isOpen())
                {
                    switch (_scene)
                    {
                        case 0:
                            menuScene(window);
                            break;
                        case 1:
                            gameScene(window, client);
                            break;
                        case 2:
                            settingsScene(window);
                            break;
                        case 3:
                            creditsScene(window);
                            break;
                        case 4:
                            endScene(window);
                            break;
                        default:
                            std::cout << "Error: Scene not found : " << _scene << std::endl;
                            break;
                    }
                }
            }

        private:
            std::string _host;
            std::string _port;
            sf::Clock _clock;
            PositionSystem posSys;
            DamageSystem dmgSys;
            ButtonSystem btnSys;
            float _updateInterval;
            Registry _reg;
            Registry _regMenu;
            Registry _regEnd;
            int createdPlayers = 0;
            rtype::Assets &_assets;
            int _scene = 0;
    };
} // namespace rtype

#endif // R_TYPE_CLIENT_HPP
