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
                _server(ioContext, ioService), _reg(Registry())
            {
            };

            ~Game() = default;

            void runNetwork()
            {
                _server.startServer();
            }

            const net::Server &getServerContext() const noexcept {return _server;}
            const Registry &getRegistry() const noexcept {return _reg;}

            void runGame() {
                _reg.register_component<Position>();
                _reg.register_component<Velocity>();
                _reg.register_component<Velocity>();
                _reg.spawn_entity();
                auto positions = _reg.get_components<Position>();
                auto velocities = _reg.get_components<Velocity>();

                velocities[0] = 345;
                velocities[1] = 642;

                std::cout << sizeof(velocities[0]) << std::endl;
                std::cout << velocities[0].value()._velocity << std::endl;

                while (_server.isSocketOpen()) {
                    std::this_thread::sleep_for(std::chrono::seconds(3));
                    if (_server.getClients().size()) {
                        std::cout << positions.size() << " " << velocities.size() << std::endl;
                        _server.sendResponse(packet::TEST_ECS, velocities[0]);
                        _server.sendResponse(packet::TEST_ECS, velocities[1]);
                    }
                }
            };

        private:
            net::Server _server;
            Registry _reg;
    };
}

#endif //R_TYPE_SERVER_HPP
