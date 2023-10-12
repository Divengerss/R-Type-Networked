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
    // Entity e = reg.spawn_entity();

    // reg.add_component<Texture>(e, {"./Release/assets/sprites/r-typesheet1.gif"});
    // reg.add_component<Position>(e, {10, 10});
    // reg.add_component<Scale>(e, {3, 3});
    reg.add_component<Texture>(Space_background, {"./Release/assets/sprites/Space.png"});
    reg.add_component<Position>(Space_background, {0, 0});
    reg.add_component<Scale>(Space_background, {5, 5});
    reg.add_component<Velocity>(Space_background, {2});
    reg.add_component<MovementPattern>(Space_background, {STRAIGHT});
    reg.add_component<Controllable>(Space_background, {false});

    Game g;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear();
        g.updateSprite(reg, window);
        window.display();
    }
    // auto positions = reg.get_components<Position>();
    // auto velocities = reg.get_components<Velocity>();
    // auto textures = reg.get_components<Texture>();

    // Entity e2 = reg.spawn_entity();

    // reg.add_component<Texture>(e2, {"test2"});
    // std::cout << positions.size() << " " << velocities.size() << reg.get_components<Texture>().size() << std::endl;
    return (0);
}