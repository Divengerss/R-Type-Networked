#ifndef R_TYPE_CLIENT_HPP
#define R_TYPE_CLIENT_HPP

#ifdef _WIN32
#define _WIN32_WINNT 0x0601
#endif /* !_WIN32 */

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <utility>

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
#include "DrawSystem.hpp"
#include "ChatBoxSystem.hpp"
#include "HealthBarSystem.hpp"
#include "SpriteSystem.hpp"
#include "TextureSystem.hpp"

namespace rtype
{
    class Game
    {
        public:
            Game(std::string host, std::string port) : _host(host), _port(port), _updateInterval(1.0f / 60), _assets(rtype::Assets::getInstance())
            {
                _regMenu.register_component<Button>();

                _regRoom.register_component<Button>();

                _regEnd.register_component<Button>();

                _regCredits.register_component<Button>();

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

            void roomScene(sf::RenderWindow &window, net::Client &client)
            {
                sf::Event event;
                sf::Time elapsedTime = _clock.getElapsedTime();
                bool mouse_clicked = false;

                if (_fetchRoomClock.getElapsedTime().asSeconds() >= 1) {
                    client.roomList();
                    _fetchRoomClock.restart();
                }

                for (int i = 0; i < _regRoom.get_entity_number(); ++i) {
                    _regRoom.kill_entity(Entity(i));
                }

                std::vector<std::pair<std::uint64_t, std::uint8_t>> rooms = client.getRooms();

                Entity buttonCreateRoom = _regRoom.spawn_entity();
                _regRoom.add_component<Button>(buttonCreateRoom,
                    {
                        static_cast<float>(window.getSize().x / 2), static_cast<float>(window.getSize().y / 5 * 4),
                        100.f, 100.f,
                        std::string("Create Room"),
                        _assets.getFont("arial"),
                        std::function<void()>([&]() { client.connect(rooms.size(), true); _scene = 1; std::cout << "Create Room" << std::endl; })
                    }
                );

                for (std::size_t i = 0; i < rooms.size(); ++i) {
                    Entity buttonRoom = _regRoom.spawn_entity();
                    _regRoom.add_component<Button>(buttonRoom,
                        {
                            static_cast<float>(window.getSize().x / 2), static_cast<float>(window.getSize().y / 5 * (i + 1)),
                            100.f, 100.f,
                            std::string("Room " + std::to_string(i) + " (max " + std::to_string(rooms[i].second) + ")"),
                            _assets.getFont("arial"),
                            std::function<void()>([&]() { client.connect(i - 1, false); _scene = 1; std::cout << "Room " << i - 1 << std::endl; })
                        }
                    );
                }

                if (elapsedTime.asSeconds() >= _updateInterval)
                {
                    // Event
                    while (window.pollEvent(event))
                    {
                        if (event.type == sf::Event::Closed)
                            window.close();
                        if (event.type == sf::Event::MouseButtonPressed)
                            if (event.mouseButton.button == sf::Mouse::Left)
                                mouse_clicked = true;
                    }

                    // Update
                    btnSys.run(_regRoom, window, mouse_clicked);

                    // Draw
                    window.clear();

                    for (int i = 0; i < _regRoom.get_entity_number(); ++i) {
                        if (_regRoom.entity_has_component<Button>(Entity(i))) {
                            auto &button = _regRoom.get_component<Button>(Entity(i));
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
                        chatSys.run(event, _reg, client);
                    }

                    // Update
                    posSys.positionSystemClient(_reg);
                    spriteSys.run(_reg, _assets);
                    textureSys.run(_reg, createdPlayers);
                    healthBarSys.run(_reg);
                    dmgSys.damageSystem(_reg);

                    // Draw
                    window.clear();

                    drawSys.run(_reg, window, _assets);

                    window.display();

                    _clock.restart();


                    if (client.isSocketOpen() == false || checkLoseCondition()) {
                        _scene = 4;
                        //client.disconnect();
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
                    btnSys.run(_regCredits, window, mouse_clicked);

                    // Draw
                    window.clear();

                    for (int i = 0; i < _regCredits.get_entity_number(); ++i) {
                        if (_regCredits.entity_has_component<Button>(Entity(i))) {
                            auto &button = _regCredits.get_component<Button>(Entity(i));
                            button.draw(window);
                        }
                    }

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
                        std::function<void()>([&]() { _scene = 5; client.run(); std::cout << "Play" << std::endl; })
                    }
                );
                Entity buttonCredits = _regMenu.spawn_entity();
                _regMenu.add_component<Button>(buttonCredits,
                    {
                        static_cast<float>(width / 2), static_cast<float>(height / 5 * 2),
                        100.f, 100.f,
                        std::string("Credits"),
                        _assets.getFont("arial"),
                        std::function<void()>([&]() { _scene = 3; std::cout << "Credits" << std::endl; })
                    }
                );
                Entity buttonQuit = _regMenu.spawn_entity();
                _regMenu.add_component<Button>(buttonQuit,
                    {
                        static_cast<float>(width / 2), static_cast<float>(height / 5 * 3),
                        100.f, 100.f,
                        std::string("Quit"),
                        _assets.getFont("arial"),
                        std::function<void()>([&]() { window.close(); std::cout << "Quit" << std::endl; })
                    }
                );

                // Ecs Credits
                Entity buttonCreditsJules = _regCredits.spawn_entity();
                _regCredits.add_component<Button>(buttonCreditsJules,
                    {
                        static_cast<float>(width / 2), static_cast<float>(height / 5 * 0),
                        100.f, 100.f,
                        std::string("Jules Levi"),
                        _assets.getFont("arial"),
                        std::function<void()>([&]() { std::cout << "Jules" << std::endl; })
                    }
                );
                Entity buttonCreditsMaxime = _regCredits.spawn_entity();
                _regCredits.add_component<Button>(buttonCreditsMaxime,
                    {
                        static_cast<float>(width / 2), static_cast<float>(height / 5 * 1),
                        100.f, 100.f,
                        std::string("Maxime Eng"),
                        _assets.getFont("arial"),
                        std::function<void()>([&]() { std::cout << "Maxime" << std::endl; })
                    }
                );
                Entity buttonCreditsDorvann = _regCredits.spawn_entity();
                _regCredits.add_component<Button>(buttonCreditsDorvann,
                    {
                        static_cast<float>(width / 2), static_cast<float>(height / 5 * 2),
                        100.f, 100.f,
                        std::string("Dorvann Ledeux"),
                        _assets.getFont("arial"),
                        std::function<void()>([&]() { std::cout << "Dorvann" << std::endl; })
                    }
                );
                Entity buttonCreditsJulian = _regCredits.spawn_entity();
                _regCredits.add_component<Button>(buttonCreditsJulian,
                    {
                        static_cast<float>(width / 2), static_cast<float>(height / 5 * 3),
                        100.f, 100.f,
                        std::string("Julian Emery"),
                        _assets.getFont("arial"),
                        std::function<void()>([&]() { std::cout << "Julian" << std::endl; })
                    }
                );
                Entity buttonCreditsMenu = _regCredits.spawn_entity();
                _regCredits.add_component<Button>(buttonCreditsMenu,
                    {
                        static_cast<float>(width / 2), static_cast<float>(height / 5 * 4),
                        100.f, 100.f,
                        std::string("Menu"),
                        _assets.getFont("arial"),
                        std::function<void()>([&]() { _scene = 0; std::cout << "Menu" << std::endl; })
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
                        case 5:
                            roomScene(window, client);
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
            sf::Clock _fetchRoomClock;
            PositionSystem posSys;
            DamageSystem dmgSys;
            ButtonSystem btnSys;
            DrawSystem drawSys;
            ChatBoxSystem chatSys;
            HealthBarSystem healthBarSys;
            SpriteSystem spriteSys;
            TextureSystem textureSys;
            float _updateInterval;
            Registry _reg;
            Registry _regMenu;
            Registry _regEnd;
            Registry _regRoom;
            Registry _regCredits;
            int createdPlayers = 0;
            rtype::Assets &_assets;
            int _scene = 0;
    };
} // namespace rtype

#endif // R_TYPE_CLIENT_HPP
