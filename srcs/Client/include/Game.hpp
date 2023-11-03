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

namespace rtype
{
    class Game
    {
    public:
        Game(std::string host, std::string port) : _host(host), _port(port), _updateInterval(1.0f / 60), _assets(rtype::Assets::getInstance())
        {
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
        }

        ~Game() = default;

        void textureSystem()
        {
            for (int i = 0; i < _reg.get_entity_number(); ++i)
            {
                if (_reg.entity_has_component<Texture>(Entity(i)))
                    continue;

                if (_reg.entity_has_component<Tag>(Entity(i)))
                {
                    auto &tag = _reg.get_component<Tag>(Entity(i));

                    switch (tag._tag)
                    {
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

        void updateSprite(net::Client &client)
        {
            textureSystem();
            posSys.positionSystemClient(_reg, client);

            for (int i = 0; i < _reg.get_entity_number(); i++)
            {
                if (!_reg.entity_has_component<Texture>(Entity(i)))
                    continue;

                auto &texture = _reg.get_component<Texture>(Entity(i));
                auto &pos = _reg.get_component<Position>(Entity(i));
                auto &scale = _reg.get_component<Scale>(Entity(i));

                if (!_reg.entity_has_component<Sprite>(Entity(i)))
                {
                    sf::Texture &spriteTexture = _assets.getTexture(_reg.get_component<Texture>(Entity(i))._path);
                    _reg.add_component<Sprite>(Entity(i), {spriteTexture});
                    auto &sprite = _reg.get_component<Sprite>(Entity(i))._sprite;

                    sprite.setPosition(pos._x, pos._y);
                    sprite.setTextureRect(sf::IntRect(texture._left, texture._top, texture._width, texture._height));
                    sprite.setScale(scale._scaleX, scale._scaleY);
                }
                else
                {
                    auto &sprite = _reg.get_component<Sprite>(Entity(i))._sprite;
                    sprite.setPosition(pos._x, pos._y);
                }
            }
            dmgSys.damageSystem(_reg);
        }

        void drawSprite(sf::RenderWindow &window)
        {
            for (int i = 0; i < _reg.get_entity_number(); i++)
            {
                if (!_reg.entity_has_component<Sprite>(Entity(i)))
                    continue;

                auto &sprite = _reg.get_component<Sprite>(Entity(i));

                window.draw(sprite._sprite);
            }
        }

        bool checkLoseCondition(mainMenu &mainMenu)
        {
            int count = 0;

            for (int i = 0; i < _reg.get_entity_number(); ++i)
            {
                if (_reg.entity_has_component<Tag>(Entity(i)) && _reg.entity_has_component<Destroyable>(Entity(i)))
                {
                    auto &tag = _reg.get_component<Tag>(Entity(i));
                    auto &destroyable = _reg.get_component<Destroyable>(Entity(i));
                    if (tag._tag == TagEnum::PLAYER && destroyable._hp > 0)
                    {
                        count++;
                    }
                }
            }

            if (count == 0 && createdPlayers > 0)
            {
                mainMenu.setMenu(4);
                return true;
            }
            return false;
        }

        void runGame()
        {
            sf::RenderWindow window(sf::VideoMode(1920, 1080), "R-Type Alpha");

            Entity Space_background = _reg.spawn_entity();
            _reg.add_component<Texture>(Space_background, {"space", 0, 0, 950, 200});
            _reg.add_component<Position>(Space_background, {0, 0});
            _reg.add_component<Scale>(Space_background, {5, 5});
            _reg.add_component<Velocity>(Space_background, {1});
            _reg.add_component<MovementPattern>(Space_background, {STRAIGHTLEFT});

            sf::Clock clock;

            mainMenu mainMenu(window.getSize().x, window.getSize().y);

            while (window.isOpen())
            {
                sf::Event event;
                sf::Time elapsedTime = _clock.getElapsedTime();
                if (elapsedTime.asSeconds() >= _updateInterval)
                {
                    while (window.pollEvent(event))
                    {
                        if (event.type == sf::Event::Closed)
                            window.close();

                        if (event.type == sf::Event::KeyReleased)
                        {
                            if (event.key.code == sf::Keyboard::Up)
                            {
                                mainMenu.MoveUp();
                                break;
                            }
                            if (event.key.code == sf::Keyboard::Down)
                            {
                                mainMenu.MoveDown();
                                break;
                            }
                            int x = mainMenu.mainMenuPressed();
                            if (event.key.code == sf::Keyboard::Return)
                            {
                                if (x == 0)
                                {
                                    asio::io_context io_context;
                                    net::Client client(io_context, _host, _port, _reg);

                                    while (window.isOpen() && client.isSocketOpen())
                                    {
                                        sf::Time elapsedTime = _clock.getElapsedTime();
                                        if (elapsedTime.asSeconds() >= _updateInterval)
                                        {
                                            while (window.pollEvent(event))
                                            {
                                                if (event.type == sf::Event::Closed)
                                                    window.close();
                                            }

                                            window.clear();
                                            updateSprite(client);

                                            drawSprite(window);

                                            window.display();

                                            _clock.restart();
                                            if (checkLoseCondition(mainMenu))
                                            {
                                                x = 4;
                                                break;
                                            }
                                        }
                                    }
                                    client.disconnect();
                                }
                                if (x == 1)
                                {
                                }
                                if (x == 2)
                                {
                                }
                                if (x == 3)
                                    window.close();
                                if (x == 4)
                                {
                                    EndMenu endMenu = mainMenu.getEndMenu();
                                    bool returnPressed = false;
                                    while (window.isOpen())
                                    {
                                        while (window.pollEvent(event))
                                        {
                                            if (event.type == sf::Event::Closed)
                                                window.close();

                                            if (event.type == sf::Event::KeyReleased)
                                            {
                                                if (event.key.code == sf::Keyboard::Up)
                                                {
                                                    endMenu.MoveUp();
                                                    break;
                                                }
                                                if (event.key.code == sf::Keyboard::Down)
                                                {
                                                    endMenu.MoveDown();
                                                    break;
                                                }
                                                if (event.key.code == sf::Keyboard::Return)
                                                {
                                                    returnPressed = true;
                                                }
                                            }
                                        }
                                        int y = endMenu.endMenuPressed();
                                        if (returnPressed)
                                        {
                                            if (y == 0)
                                            {
                                                mainMenu.setMenu(0);
                                                break;
                                            }
                                            if (y == 1)
                                            {
                                                window.close();
                                                break;
                                            }
                                        }
                                        window.clear();
                                        endMenu.draw(window);
                                        window.display();
                                        clock.restart();
                                    }
                                }
                                break;
                            }
                        }
                    }
                    window.clear();
                    mainMenu.draw(window);
                    window.display();
                    _clock.restart();
                }
            }
        }

    private:
        std::string _host;
        std::string _port;
        sf::Clock _clock;
        PositionSystem posSys;
        DamageSystem dmgSys;
        float _updateInterval;
        Registry _reg;
        int createdPlayers = 0;
        rtype::Assets &_assets;
    };
} // namespace rtype

#endif // R_TYPE_CLIENT_HPP
