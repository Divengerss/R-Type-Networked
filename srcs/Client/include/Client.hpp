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
#include "Tag.hpp"

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

                Entity Space_background = _reg.spawn_entity();
                #ifdef WIN32
                    _reg.add_component<Texture>(Space_background, {"Release\\assets\\sprites\\Space.png", 0, 0, 950, 200});
                #else
                    _reg.add_component<Texture>(Space_background, {"./Release/assets/sprites/Space.png", 0, 0, 950, 200});
                #endif
                _reg.add_component<Position>(Space_background, {0, 0});
                _reg.add_component<Scale>(Space_background, {5, 5});
                _reg.add_component<Velocity>(Space_background, {1});
                _reg.add_component<MovementPattern>(Space_background, {STRAIGHTLEFT});
            }
            ~Game() = default;

            void textureSystem() {
                auto positions = _reg.get_components<Position>();
                auto controllable = _reg.get_components<Controllable>();
                auto textures = _reg.get_components<Texture>();
                auto movements = _reg.get_components<MovementPattern>();

                for (std::size_t i = 0; i < positions.size(); i++) {
                    auto &texture = textures[i];
                    auto &pos = positions[i];
                    auto &cont = controllable[i];
                    auto &move = movements[i];
                    if (pos && cont && !texture.has_value()) {
                        #ifdef WIN32
                            _reg.add_component<Texture>(Entity(i), {"Release\\assets\\sprites\\r-typesheet42.gif", 66, createdPlayers * 18, 33, 17});
                        #else
                            _reg.add_component<Texture>(Entity(i), {"./Release/assets/sprites/r-typesheet42.gif", 66, createdPlayers * 18, 33, 17});
                        #endif
                        _reg.add_component<Scale>(Entity(i), {3, 3});
                        createdPlayers++;
                    }
                    else if (pos && !cont.has_value() && !texture.has_value()) {
                        if (move->_movementPattern == MovementPatterns::STRAIGHTLEFT)
                            #ifdef WIN32
                                _reg.add_component<Texture>(Entity(i), {"Release\\assets\\sprites\\r-typesheet5.gif", 233, 0, 33, 36});
                            #else
                                _reg.add_component<Texture>(Entity(i), {"./Release/assets/sprites/r-typesheet5.gif", 233, 0, 33, 36});
                            #endif
                        if (move->_movementPattern == MovementPatterns::STRAIGHTRIGHT)
                            #ifdef WIN32
                                _reg.add_component<Texture>(Entity(i), {"Release\\assets\\sprites\\r-typesheet2.gif", 185, 0, 25, 25});
                                #else
                                _reg.add_component<Texture>(Entity(i), {"./Release/assets/sprites/r-typesheet2.gif", 185, 0, 25, 25});
                                #endif

                        _reg.add_component<Scale>(Entity(i), {3, 3});
                    }
                }
            }

            void updateSprite()
            {
                textureSystem();
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
            };

            void drawSprite(sf::RenderWindow &window)
            {
                for (auto &sprite : _sprites)
                    window.draw(sprite.second.first);
            }

            bool checkLoseCondition(Registry &t, mainMenu &mainMenu)
            {
                int count = 0;
                auto tags = t.get_components<Tag>();

                for (std::size_t i = 0; i < tags.size(); ++i)
                {
                    auto &tag = tags[i];
                    if (tag && tag->_tag == "player")
                    {
                        count++;
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
                reg.register_component<Tag>();

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
                reg.add_component<Tag>(e, {"player"});

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
                                                if (checkLoseCondition(reg, mainMenu)) {
                                                    x = 4;
                                                    break;
                                                }

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
                _client.disconnect();
            }

            private:
                net::Client &_client;
                std::map<size_t, std::pair<sf::Sprite, sf::Texture>> _sprites;
                sf::Clock _clock;
                PositionSystem posSys;
                DamageSystem dmgSys;
                float _updateInterval;
                Registry &_reg;
                int createdPlayers = 0;
    };
} // namespace rtype

#endif //R_TYPE_CLIENT_HPP
