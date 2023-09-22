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

            void runNetwork()
            {
                _server.startServer();
                std::cout << "Server running at " << _server.getHost() << " with port " << _server.getPort() << std::endl;
            
            }

            const net::Server &getServerContext() const noexcept {return _server;}

            void runGame() {
                while (true) { // TO CHANGE
                }
            };

        private:
            net::Server _server;
    };
}

#endif //R_TYPE_SERVER_HPP
