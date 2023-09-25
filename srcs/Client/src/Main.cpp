#include "Client.hpp"
#include "Netcode.hpp"
#include "CFGParser.hpp"

int main() {
    utils::ParseCFG config(utils::getCurrDir() + CLIENT_CONFIG_FILE_PATH);
    asio::io_context ioContext;
    try {
        std::string host = config.getData<std::string>("host");
        std::string port = config.getData<std::string>("port");
        net::Client client(ioContext, host, port);
        client.connect();
        client.disconnect();
    } catch (const Error &e) {
        std::cerr << e.what() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
