#include "Client.hpp"


int main()
{
    utils::ParseCFG config(utils::getCurrDir() + clientConfigFilePath.data());
    asio::io_context ioContext;
    try {
        std::string host = config.getData<std::string>("host");
        std::string port = config.getData<std::string>("port");
        net::Client client(ioContext, host, port);
        rtype::Game game(client);
        game.runGame();
    } catch (const Error &e) {
        std::cerr << e.what() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
