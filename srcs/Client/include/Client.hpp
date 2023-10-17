#ifndef R_TYPE_CLIENT_HPP
#define R_TYPE_CLIENT_HPP

#ifdef _WIN32
    #define _WIN32_WINNT 0x0601
#endif /* !_WIN32 */

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

namespace rtype
{
    class Game
    {
        public:
            Game() = default;
            Game(net::Client &client, Registry &reg) : _client(client), _updateInterval(1.0f/60), _reg(reg)
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
            }
            ~Game() = default;

            void updateSprite()
            {
                posSys.positionSystemClient(_reg, _client);
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
                        sf::Texture spriteTexture(texture->_texture);
                        sprite.setTexture(spriteTexture);
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
            };

            void drawSprite(sf::RenderWindow &window)
            {
                for (auto &sprite : _sprites)
                    window.draw(sprite.second.first);
            }

            void runGame()
            {
                sf::RenderWindow window(sf::VideoMode(1920, 1080), "R-Type Alpha");

                Entity Space_background = _reg.spawn_entity();
                _reg.add_component<Texture>(Space_background, {"./Release/assets/sprites/Space.png", 0, 0, 950, 200});
                _reg.add_component<Position>(Space_background, {0, 0});
                _reg.add_component<Scale>(Space_background, {5, 5});
                _reg.add_component<Velocity>(Space_background, {1});
                _reg.add_component<MovementPattern>(Space_background, {STRAIGHTLEFT});
                // _reg.add_component<Destroyable>(Space_background, {false});
                _reg.add_component<Hitbox>(Space_background, {950, 200});
                // _reg.add_component<Damaging>(Space_background, {false});

                Entity e = _reg.spawn_entity();
                _reg.add_component<Texture>(e, {"./Release/assets/sprites/r-typesheet42.gif", 66, 0, 33, 17});
                _reg.add_component<Position>(e, {10, 10});
                _reg.add_component<Scale>(e, {3, 3});
                _reg.add_component<Velocity>(e, {10});
                _reg.add_component<MovementPattern>(e, {NONE});
                _reg.add_component<Controllable>(e, {" "});
                _reg.add_component<Destroyable>(e, {3});
                _reg.add_component<Hitbox>(e, {33, 17});
                // _reg.add_component<Damaging>(e, {false});

                // Entity e2 = _reg.spawn_entity();
                // _reg.add_component<Texture>(e2, {"./Release/assets/sprites/r-typesheet42.gif", 66, 0, 33, 17});
                // _reg.add_component<Position>(e2, {10, 10});
                // _reg.add_component<Scale>(e2, {3, 3});
                // _reg.add_component<Velocity>(e2, {1});
                // _reg.add_component<MovementPattern>(e2, {NONE});
                // _reg.add_component<Controllable>(e2, {false});
                // // _reg.add_component<Destroyable>(e2, {true});
                // // _reg.add_component<Hitbox>(e2, {33, 17});
                // // _reg.add_component<Damaging>(e2, {false});

                // Entity e3 = _reg.spawn_entity();
                // _reg.add_component<Texture>(e3, {"./Release/assets/sprites/r-typesheet42.gif", 66, 0, 33, 17});
                // _reg.add_component<Position>(e3, {10, 10});
                // _reg.add_component<Scale>(e3, {3, 3});
                // _reg.add_component<Velocity>(e3, {1});
                // _reg.add_component<MovementPattern>(e3, {NONE});
                // _reg.add_component<Controllable>(e3, {false});

                // Entity e4 = _reg.spawn_entity();
                // _reg.add_component<Texture>(e4, {"./Release/assets/sprites/r-typesheet42.gif", 66, 0, 33, 17});
                // _reg.add_component<Position>(e4, {10, 10});
                // _reg.add_component<Scale>(e4, {3, 3});
                // _reg.add_component<Velocity>(e4, {1});
                // _reg.add_component<MovementPattern>(e4, {NONE});
                // _reg.add_component<Controllable>(e4, {false});

                Entity monster = _reg.spawn_entity();
                _reg.add_component<Texture>(monster, {"./Release/assets/sprites/r-typesheet5.gif", 233, 0, 33, 36});
                _reg.add_component<Position>(monster, {1920, 500});
                _reg.add_component<Scale>(monster, {3, 3});
                _reg.add_component<Velocity>(monster, {2});
                _reg.add_component<MovementPattern>(monster, {STRAIGHTLEFT});
                _reg.add_component<Destroyable>(monster, {2});
                _reg.add_component<Hitbox>(monster, {33, 17});
                _reg.add_component<Damaging>(monster, {true});

                mainMenu mainMenu(window.getSize().x, window.getSize().y);

                while (window.isOpen() && _client.isSocketOpen())
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
                                        while (window.isOpen()) {
                                            sf::Time elapsedTime = _clock.getElapsedTime();
                                            if (elapsedTime.asSeconds() >= _updateInterval)
                                            {
                                                while (window.pollEvent(event)) {
                                                    if (event.type == sf::Event::Closed)
                                                        window.close();
                                                }
                                                window.clear();
                                                updateSprite();
                                                drawSprite(window);
                                                window.display();
                                                _clock.restart();
                                            }
                                        }
                                    }
                                    if (x == 1)
                                    {
                                    }
                                    if (x == 2)
                                    {
                                    }
                                    if (x == 3)
                                        window.close();
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
                net::Client &_client;
                std::map<size_t, std::pair<sf::Sprite, sf::Texture>> _sprites;
                sf::Clock _clock;
                PositionSystem posSys;
                DamageSystem dmgSys;
                float _updateInterval;
                Registry &_reg;
    };
} // namespace rtype

#endif //R_TYPE_CLIENT_HPP
