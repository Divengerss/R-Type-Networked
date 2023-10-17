#ifndef R_TYPE_SERVER_HPP
#define R_TYPE_SERVER_HPP

#ifdef _WIN32
    #define _WIN32_WINNT 0x0601
#endif /* !_WIN32 */

#include "Network.hpp"
#include "Registry.hpp"
#include "Position.hpp"
#include "Velocity.hpp"
#include "Hitbox.hpp"
#include "Controllable.hpp"
#include "Server/PositionSystem.hpp"
#include "Scale.hpp"
#include "Destroyable.hpp"
#include <thread>
#include <chrono>

namespace rtype
{
    class Game
    {
        public:
            Game() = delete;
            Game(asio::io_context &ioContext, asio::io_context &ioService) :
                _reg(Registry()), _pos(), _server(ioContext, ioService, _reg)
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
                _reg.register_component<Hitbox>();
                _reg.register_component<Controllable>();
                _reg.register_component<MovementPattern>();
                _reg.register_component<Destroyable>();
                _reg.register_component<Damaging>();

                _reg.spawn_entity(); //Background index
                // Entity monster = _reg.spawn_entity();
                // _reg.add_component<Position>(monster, {1920, 500});
                // _reg.add_component<Velocity>(monster, {2});
                // _reg.add_component<MovementPattern>(monster, {STRAIGHTLEFT});
                // _reg.add_component<Destroyable>(monster, {2});
                // _reg.add_component<Hitbox>(monster, {33, 17});
                // _reg.add_component<Damaging>(monster, {true});

                while (_server.isSocketOpen()) {
                    // std::this_thread::sleep_for(std::chrono::seconds(1));
                    if (_server.getClients().size()) {
                        _pos.positionSystemServer(_reg);
                        auto &positions = _reg.get_components<Position>();
                        for (auto &component : positions) {
                            if (component.has_value()) {
                                std::cout << "X = " << component.value()._x << " Y = " << component.value()._y << std::endl;
                            } else
                                std::cout << "nullopt" << std::endl;
                        }
                        _server.sendSparseArray<Position>(packet::ECS_POSITION, positions);
                    }
                }
            };

        private:
            Registry _reg;
            PositionSystem _pos;
            net::Server _server;
    };
}

#endif //R_TYPE_SERVER_HPP
