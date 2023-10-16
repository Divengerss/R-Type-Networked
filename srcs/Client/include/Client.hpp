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
            Game(net::Client &client) : _client(client) {}
            ~Game() = default;

            void updateSprite(Registry &t)
            {
                p.positionSystem(t);
                auto positions = t.get_components<Position>();
                auto velocities = t.get_components<Velocity>();
                auto textures = t.get_components<Texture>();
                auto scales = t.get_components<Scale>();

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
                d.damageSystem(t, _sprites);
            };

            void drawSprite(sf::RenderWindow &window)
            {
                for (auto &sprite : _sprites)
                    window.draw(sprite.second.first);
            }

            void runGame()
            {
                sf::RenderWindow window(sf::VideoMode(1920, 1080), "SFML window");

                Registry reg;
                reg.register_component<Position>();
                reg.register_component<Velocity>();
                reg.register_component<Texture>();
                reg.register_component<Scale>();
                reg.register_component<MovementPattern>();
                reg.register_component<Controllable>();
                reg.register_component<Destroyable>();
                reg.register_component<Hitbox>();
                reg.register_component<Damaging>();

                Entity Space_background = reg.spawn_entity();
                reg.add_component<Texture>(Space_background, {"./Release/assets/sprites/Space.png", 0, 0, 950, 200});
                reg.add_component<Position>(Space_background, {0, 0});
                reg.add_component<Scale>(Space_background, {5, 5});
                reg.add_component<Velocity>(Space_background, {1});
                reg.add_component<MovementPattern>(Space_background, {STRAIGHTLEFT});
                // reg.add_component<Destroyable>(Space_background, {false});
                reg.add_component<Hitbox>(Space_background, {950, 200});
                // reg.add_component<Damaging>(Space_background, {false});

                Entity e = reg.spawn_entity();
                reg.add_component<Texture>(e, {"./Release/assets/sprites/r-typesheet42.gif", 66, 0, 33, 17});
                reg.add_component<Position>(e, {10, 10});
                reg.add_component<Scale>(e, {3, 3});
                reg.add_component<Velocity>(e, {10});
                reg.add_component<MovementPattern>(e, {NONE});
                reg.add_component<Controllable>(e, {" "});
                reg.add_component<Destroyable>(e, {3});
                reg.add_component<Hitbox>(e, {33, 17});
                // reg.add_component<Damaging>(e, {false});

                // Entity e2 = reg.spawn_entity();
                // reg.add_component<Texture>(e2, {"./Release/assets/sprites/r-typesheet42.gif", 66, 0, 33, 17});
                // reg.add_component<Position>(e2, {10, 10});
                // reg.add_component<Scale>(e2, {3, 3});
                // reg.add_component<Velocity>(e2, {1});
                // reg.add_component<MovementPattern>(e2, {NONE});
                // reg.add_component<Controllable>(e2, {false});
                // // reg.add_component<Destroyable>(e2, {true});
                // // reg.add_component<Hitbox>(e2, {33, 17});
                // // reg.add_component<Damaging>(e2, {false});

                // Entity e3 = reg.spawn_entity();
                // reg.add_component<Texture>(e3, {"./Release/assets/sprites/r-typesheet42.gif", 66, 0, 33, 17});
                // reg.add_component<Position>(e3, {10, 10});
                // reg.add_component<Scale>(e3, {3, 3});
                // reg.add_component<Velocity>(e3, {1});
                // reg.add_component<MovementPattern>(e3, {NONE});
                // reg.add_component<Controllable>(e3, {false});

                // Entity e4 = reg.spawn_entity();
                // reg.add_component<Texture>(e4, {"./Release/assets/sprites/r-typesheet42.gif", 66, 0, 33, 17});
                // reg.add_component<Position>(e4, {10, 10});
                // reg.add_component<Scale>(e4, {3, 3});
                // reg.add_component<Velocity>(e4, {1});
                // reg.add_component<MovementPattern>(e4, {NONE});
                // reg.add_component<Controllable>(e4, {false});

                Entity monster = reg.spawn_entity();
                reg.add_component<Texture>(monster, {"./Release/assets/sprites/r-typesheet5.gif", 233, 0, 33, 36});
                reg.add_component<Position>(monster, {1920, 500});
                reg.add_component<Scale>(monster, {3, 3});
                reg.add_component<Velocity>(monster, {2});
                reg.add_component<MovementPattern>(monster, {STRAIGHTLEFT});
                reg.add_component<Destroyable>(monster, {2});
                reg.add_component<Hitbox>(monster, {33, 17});
                reg.add_component<Damaging>(monster, {true});

                sf::Clock clock;
                float updateInterval = 1.0f/60;
                mainMenu mainMenu(window.getSize().x, window.getSize().y);

                while (window.isOpen() && _client.isSocketOpen())
                {
                    sf::Event event;
                    sf::Time elapsedTime = clock.getElapsedTime();
                    if (elapsedTime.asSeconds() >= updateInterval)
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
                                            sf::Time elapsedTime = clock.getElapsedTime();
                                            if (elapsedTime.asSeconds() >= updateInterval)
                                            {
                                                while (window.pollEvent(event)) {
                                                    if (event.type == sf::Event::Closed)
                                                        window.close();
                                                }
                                                window.clear();
                                                updateSprite(reg);
                                                drawSprite(window);
                                                window.display();
                                                clock.restart();
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
                        clock.restart();
                    }
                }
            }

            private:
                net::Client &_client;
                std::map<size_t, std::pair<sf::Sprite, sf::Texture>> _sprites;
                sf::Clock Clock;
                PositionSystem p;
                DamageSystem d;
                float updateInterval = 1.0f;
    };
} // namespace rtype

#endif //R_TYPE_CLIENT_HPP
