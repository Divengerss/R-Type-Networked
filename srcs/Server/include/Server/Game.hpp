#ifndef R_TYPE_SERVER_HPP
#define R_TYPE_SERVER_HPP

#ifdef _WIN32
    #define _WIN32_WINNT 0x0601
#endif /* !_WIN32 */

#include "Network.hpp"
#include "Registry.hpp"
#include "Position.hpp"
#include "Velocity.hpp"

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
                Registry reg;

                reg.register_component<Position>();
                reg.register_component<Velocity>();
                reg.spawn_entity();
                auto positions = reg.get_components<Position>();
                auto velocities = reg.get_components<Velocity>();

                while (true) {
                    if (_server.getClients().size()) {
                        std::cout << positions.size() << " " << velocities.size() << std::endl;
                    }
                }
            };

        private:
            net::Server _server;
    };
}

#endif //R_TYPE_SERVER_HPP
