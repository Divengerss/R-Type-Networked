#include "Game.hpp"
#include "Error.hpp"
#include <iostream>
#include <filesystem>

#include "Engine.hpp"

int main() {
    asio::io_context ioContext;
    asio::io_service ioService;
    rtype::Engine engine;
    try {
        rtype::loopSystem game(ioContext, ioService);
        game.runNetwork();
        game.runGame();
    } catch (const Error &e) {
        std::cerr << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    } catch (const std::runtime_error &) {
        std::exit(EXIT_FAILURE);
    } catch (const std::exception &e) {
        std::cerr << "Error " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}
