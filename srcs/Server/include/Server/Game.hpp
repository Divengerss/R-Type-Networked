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
                Velocity c(642);
                Position position1(4.5f, 7.9f);
                Entity entity(0);
                _reg.add_component<Velocity>(entity, c);
                _reg.add_component<Position>(entity, position1);
                _reg.spawn_entity();
                Velocity c2(444);
                Entity entity2(1);
                _reg.add_component<Velocity>(entity2, c2);
                _reg.spawn_entity();
                Velocity c3(12345);
                Entity entity3(2);
                _reg.add_component<Velocity>(entity3, c3);
                _reg.spawn_entity();
                auto &positions = _reg.get_components<Position>();
                auto &velocities = _reg.get_components<Velocity>();

                for (auto &entityId : velocities)
                    if (entityId.has_value())
                        std::cout << entityId.value()._velocity << std::endl;
                for (auto &entityId : positions)
                    if (entityId.has_value())
                        std::cout << entityId.value()._x << " " << entityId.value()._y << std::endl;

                while (_server.isSocketOpen()) {
                    std::this_thread::sleep_for(std::chrono::seconds(3));
                    if (_server.getClients().size()) {
                        //_server.sendSparseArray<Velocity>(packet::ECS_VELOCITY, velocities);
                        _server.sendSparseArray<Position>(packet::ECS_POSITION, positions);
                    }
                }
            };

        private:
            net::Server _server;
            Registry _reg;
    };
}

#endif //R_TYPE_SERVER_HPP
