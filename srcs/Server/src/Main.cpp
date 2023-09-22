#include "Game.hpp"
#include "Error.hpp"
#include <iostream>

int main() {
    asio::io_context ioContext;
    asio::io_service ioService;
    try {
        rtype::Game game(ioContext, ioService);
        asio::signal_set signals(ioService, SIGINT);
        // signals.async_wait(srv.forceShutdown);
        // std::thread thread_context(srv.asioContextRun, std::ref(ioContext), std::ref(srv));
        // std::thread thread_service(srv.asioServiceRun, std::ref(ioContext), std::ref(ioService));
    } catch (const Error &e) {
        std::cerr << e.what() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error " << e.what() << std::endl;
    }
}
