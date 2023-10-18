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

                Entity Space_background = _reg.spawn_entity();
                _reg.add_component<Texture>(Space_background, {"./Release/assets/sprites/Space.png", 0, 0, 950, 200});
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
                        _reg.add_component<Texture>(Entity(i), {"./Release/assets/sprites/r-typesheet42.gif", 66, createdPlayers * 18, 33, 17});
                        _reg.add_component<Scale>(Entity(i), {3, 3});
                        createdPlayers++;
                    }
                    else if (pos && !cont.has_value() && !texture.has_value()) {
                        if (move->_movementPattern == MovementPatterns::STRAIGHTLEFT)
                            _reg.add_component<Texture>(Entity(i), {"./Release/assets/sprites/r-typesheet5.gif", 233, 0, 33, 36});
                        if (move->_movementPattern == MovementPatterns::STRAIGHTRIGHT)
                            _reg.add_component<Texture>(Entity(i), {"./Release/assets/sprites/r-typesheet2.gif", 185, 0, 25, 25});
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

            void runGame()
            {
                sf::RenderWindow window(sf::VideoMode(1920, 1080), "R-Type Alpha");
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
                                                checkVictory();
                                                window.clear();
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
