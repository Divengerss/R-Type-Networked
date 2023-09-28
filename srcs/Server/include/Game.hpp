#ifndef R_TYPE_SERVER_HPP
#define R_TYPE_SERVER_HPP

#include "Netcode.hpp"

#include <thread>
#include <chrono>

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
            }

            const net::Server &getServerContext() const noexcept {return _server;}

            void runGame() {
                //packet::packetHeader header(packet::PLACEHOLDER, 0);
                while (true) { // TO CHANGE
                    if (_server.getClients().size()) {
                        //std::this_thread::sleep_for(std::chrono::seconds(2));
                        //_server.sendResponse(packet::PLACEHOLDER, header);
                    }
                }
            };

        private:
            net::Server _server;
    };
}

#endif //R_TYPE_SERVER_HPP
