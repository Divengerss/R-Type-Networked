#include "Client.hpp"
#include "Registry.hpp"
#include "Position.hpp"
#include "Velocity.hpp"
#include "Texture.hpp"
#include "Game.hpp"
#include "Scale.hpp"
#include "MovementPattern.hpp"

int main()
{
    // utils::ParseCFG config(utils::getCurrDir() + clientConfigFilePath.data());
    // asio::io_context ioContext;
    // try {
    //     std::string host = config.getData<std::string>("host");
    //     std::string port = config.getData<std::string>("port");
    //     net::Client client(ioContext, host, port);
    //     rtype::Game game(client);
    //     game.runGame();
    // } catch (const Error &e) {
    //     std::cerr << e.what() << std::endl;
    // } catch (const std::exception &e) {
    //     std::cerr << e.what() << std::endl;
    // }
    // return 0;
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "SFML window");

    Registry reg;
    reg.register_component<Position>();
    reg.register_component<Velocity>();
    reg.register_component<Texture>();
    reg.register_component<Scale>();
    reg.register_component<MovementPattern>();
    reg.register_component<Controllable>();

    Entity Space_background = reg.spawn_entity();
    reg.add_component<Texture>(Space_background, {"./Release/assets/sprites/Space.png", 0, 0, 950, 200});
    reg.add_component<Position>(Space_background, {0, 0});
    reg.add_component<Scale>(Space_background, {5, 5});
    reg.add_component<Velocity>(Space_background, {2});
    reg.add_component<MovementPattern>(Space_background, {STRAIGHTLEFT});
    reg.add_component<Controllable>(Space_background, {false});

    Entity e = reg.spawn_entity();
    reg.add_component<Texture>(e, {"./Release/assets/sprites/r-typesheet42.gif", 66, 0, 33, 17});
    reg.add_component<Position>(e, {10, 10});
    reg.add_component<Scale>(e, {3, 3});
    reg.add_component<Velocity>(e, {1});
    reg.add_component<MovementPattern>(e, {NONE});
    reg.add_component<Controllable>(e, {true});

    Entity monster = reg.spawn_entity();
    reg.add_component<Texture>(monster, {"./Release/assets/sprites/r-typesheet5.gif", 233, 0, 33, 36});
    reg.add_component<Position>(monster, {1920, 500});
    reg.add_component<Scale>(monster, {3, 3});
    reg.add_component<Velocity>(monster, {1});
    reg.add_component<MovementPattern>(monster, {STRAIGHTLEFT});
    reg.add_component<Controllable>(monster, {false});

    Game g;
    sf::Clock clock;
    float updateInterval = 0.01f;
    while (window.isOpen())
    {
         sf::Time elapsedTime = clock.getElapsedTime();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        if (elapsedTime.asSeconds() >= updateInterval)
        {
            window.clear();
            g.updateSprite(reg, window);
            window.display();
            clock.restart();
        }
    }
    // auto positions = reg.get_components<Position>();
    // auto velocities = reg.get_components<Velocity>();
    // auto textures = reg.get_components<Texture>();

    // Entity e2 = reg.spawn_entity();

    // reg.add_component<Texture>(e2, {"test2"});
    // std::cout << positions.size() << " " << velocities.size() << reg.get_components<Texture>().size() << std::endl;
    return (0);
}