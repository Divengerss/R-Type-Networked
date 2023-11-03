#include "Game.hpp"

int main()
{
    utils::ParseCFG config(utils::getCurrDir() + clientConfigFilePath.data());
    try {
        std::string host = config.getData<std::string>("host");
        std::string port = config.getData<std::string>("port");
        rtype::Game game(host, port);
        game.runGame();
    } catch (const Error &e) {
        std::cerr << e.what() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
