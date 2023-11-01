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
#include "WaveSystem.hpp"

namespace rtype
{
    class Game
    {
        public:

            Game(std::string host, std::string port): _host(host), _port(port), _updateInterval(1.0f/60)
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

                Entity Space_background = _reg.spawn_entity();
                #ifdef WIN32
                    _reg.add_component<Texture>(Space_background, {"assets\\sprites\\Space.png", 0, 0, 950, 200});
                #else
                    _reg.add_component<Texture>(Space_background, {"./assets/sprites/Space.png", 0, 0, 950, 200});
                #endif
                _reg.add_component<Position>(Space_background, {0, 0});
                _reg.add_component<Scale>(Space_background, {5, 5});
                _reg.add_component<Velocity>(Space_background, {1});
                _reg.add_component<MovementPattern>(Space_background, {STRAIGHTLEFT});
            }

            ~Game() = default;

            void textureSystem()
            {
                for (int i = 0; i < _reg.get_entity_number(); ++i)
                {
                    if (_reg.entity_has_component<Texture>(Entity(i))) continue;

                    if (_reg.entity_has_component<Tag>(Entity(i))) {
                        auto &tag = _reg.get_component<Tag>(Entity(i));

                        if (tag._tag == TagEnum::PLAYER) {
                            std::cout << "Creating player" << std::endl;
                            std::cout << "Created players: " << createdPlayers << std::endl;
                            #ifdef WIN32
                                _reg.add_component<Texture>(Entity(i), {"assets\\sprites\\r-typesheet42.gif", 66, createdPlayers * 18, 33, 17});
                            #else
                                _reg.add_component<Texture>(Entity(i), {"assets/sprites/r-typesheet42.gif", 66, createdPlayers * 18, 33, 17});
                            #endif
                            _reg.add_component<Scale>(Entity(i), {3, 3});
                            createdPlayers++;
                        }
                    } else if (_reg.entity_has_component<Position>(Entity(i)) &&
                        _reg.entity_has_component<MovementPattern>(Entity(i))) {
                        auto &move = _reg.get_component<MovementPattern>(Entity(i));

                        std::cout << "Creating monster" << std::endl;

                        if (move._movementPattern == MovementPatterns::STRAIGHTLEFT)
                            #ifdef WIN32
                                _reg.add_component<Texture>(Entity(i), {"assets\\sprites\\r-typesheet5.gif", 233, 0, 33, 36});
                            #else
                                _reg.add_component<Texture>(Entity(i), {"assets/sprites/r-typesheet5.gif", 233, 0, 33, 36});
                            #endif
                        if (move._movementPattern == MovementPatterns::STRAIGHTRIGHT)
                            #ifdef WIN32
                                _reg.add_component<Texture>(Entity(i), {"assets\\sprites\\r-typesheet2.gif", 185, 0, 25, 25});
                            #else
                                _reg.add_component<Texture>(Entity(i), {"assets/sprites/r-typesheet2.gif", 185, 0, 25, 25});
                            #endif

                        _reg.add_component<Scale>(Entity(i), {3, 3});
                    }
                }
            }

            void updateSprite(net::Client &client)
            {
                textureSystem();
                posSys.positionSystemClient(_reg, client);
                auto positions = _reg.get_components<Position>();
                auto velocities = _reg.get_components<Velocity>();
                auto textures = _reg.get_components<Texture>();
                auto scales = _reg.get_components<Scale>();
                for (std::size_t i = 0; i < textures.size(); ++i)
                {
                    auto &texture = textures[i];
                    auto &pos = positions[i];
                    auto scale = scales[i];
                    if (texture && pos && _sprites.find(i) == _sprites.end())
                    {
                        sf::Sprite sprite;
                        sf::Texture spriteTexture = sf::Texture();
                        spriteTexture.loadFromFile(texture->_path);
                        sprite.setPosition(pos->_x, pos->_y);
                        sprite.setTextureRect(sf::IntRect(texture->_left, texture->_top, texture->_width, texture->_height));
                        sprite.setScale(scale->_scaleX, scale->_scaleY);
                        _sprites.emplace(i, std::pair<sf::Sprite, sf::Texture>(sprite, spriteTexture));
                        _sprites[i].first.setTexture(_sprites[i].second);
                    } else if (texture && pos) {
                        sf::Sprite &sprite = _sprites.at(i).first;
                        sprite.setPosition(pos->_x, pos->_y);
                    }
                }
                dmgSys.damageSystem(_reg, _sprites);
            }

            void drawSprite(sf::RenderWindow &window)
            {
                for (auto &sprite : _sprites)
                    window.draw(sprite.second.first);
            }

            bool checkLoseCondition(mainMenu &mainMenu)
            {
                int count = 0;

                for (int i = 0; i < _reg.get_entity_number(); ++i)
                {
                    if (_reg.entity_has_component<Tag>(Entity(i)) && _reg.entity_has_component<Destroyable>(Entity(i))) {
                        auto &tag = _reg.get_component<Tag>(Entity(i));
                        auto &destroyable = _reg.get_component<Destroyable>(Entity(i));
                        if (tag._tag == TagEnum::PLAYER && destroyable._hp > 0)
                        {
                            count++;
                        }
                    }
                }

                if (count == 0)
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
                _reg.add_component<Texture>(Space_background, {"./Build/bin/assets/sprites/Space.png", 0, 0, 950, 200});
                _reg.add_component<Position>(Space_background, {0, 0});
                _reg.add_component<Scale>(Space_background, {5, 5});
                _reg.add_component<Velocity>(Space_background, {1});
                _reg.add_component<MovementPattern>(Space_background, {STRAIGHTLEFT});
                _reg.add_component<Hitbox>(Space_background, {950, 200});

                Entity e = _reg.spawn_entity();
                _reg.add_component<Texture>(e, {"./Build/bin/assets/sprites/r-typesheet42.gif", 66, 0, 33, 17});
                _reg.add_component<Position>(e, {10, 10});
                _reg.add_component<Scale>(e, {3, 3});
                _reg.add_component<Velocity>(e, {10});
                _reg.add_component<MovementPattern>(e, {NONE});
                _reg.add_component<Controllable>(e, {" "});
                _reg.add_component<Destroyable>(e, {3});
                _reg.add_component<Hitbox>(e, {33, 17});
                _reg.add_component<Tag>(e, {TagEnum::PLAYER});

                sf::Clock clock;

                //float updateInterval = 1.0f/60;

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

                                        while (window.isOpen() && client.isSocketOpen()) {
                                            sf::Time elapsedTime = _clock.getElapsedTime();
                                            if (elapsedTime.asSeconds() >= _updateInterval)
                                            {
                                                while (window.pollEvent(event)) {
                                                    if (event.type == sf::Event::Closed)
                                                        window.close();
                                                }
                                                window.clear();
                                                updateSprite(client);
                                                drawSprite(window);

                                                waveSys.run(_reg);

                                                window.display();

                                                _clock.restart();
                                                if (checkLoseCondition(mainMenu)) {
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
                                        while (window.isOpen()) {
                                            while (window.pollEvent(event)) {
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
                std::map<size_t, std::pair<sf::Sprite, sf::Texture>> _sprites;
                sf::Clock _clock;
                PositionSystem posSys;
                DamageSystem dmgSys;
                WaveSystem waveSys;
                float _updateInterval;
                Registry _reg;
                int createdPlayers = 0;
    };
} // namespace rtype

#endif //R_TYPE_CLIENT_HPP
