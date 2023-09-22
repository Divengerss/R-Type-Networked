#ifndef R_TYPE_SERVER_HPP
#define R_TYPE_SERVER_HPP

#include "Netcode.hpp"

namespace rtype
{
    class Game
    {
        public:
            Game() = delete;
            Game(asio::io_context &ioContext, asio::io_context &ioService) :
                _server(ioContext, ioService)
            {
            };

            ~Game() = default;

            void runNetwork(asio::io_context &ioContext, asio::io_context &ioService, Game &game)
            {
                asio::signal_set signals(ioService, SIGINT);
                signals.async_wait(getServerContext().forceShutdown);
                std::thread thread_context(net::Server::asioContextRun, std::ref(ioContext));
                std::thread thread_service(net::Server::asioServiceRun, std::ref(ioContext), std::ref(ioService));
            }

            const net::Server &getServerContext() const noexcept {return _server;}

        private:
            net::Server _server;
    };
}

#endif //R_TYPE_SERVER_HPP
