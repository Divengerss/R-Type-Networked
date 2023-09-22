#include "Game.hpp"
#include "Error.hpp"
#include <iostream>
#include <filesystem>

int main(int argc, const char *argv[]) {
    asio::io_context ioContext;
    asio::io_service ioService;
    try {
        rtype::Game game(ioContext, ioService);
        game.runNetwork();
        game.runGame();
    } catch (const Error &e) {
        std::cerr << e.what() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error " << e.what() << std::endl;
    }
}
