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
#include "Damaging.hpp"
#include "Destroyable.hpp"
#include "Controllable.hpp"
#include "Entity.hpp"
#include "Server/PositionSystem.hpp"
#include "Server/DamageSystem.hpp"
#include "Destroyable.hpp"
#include "Score.hpp"
#include "Tag.hpp"
#include <thread>
#include <chrono>

namespace rtype
{
    class loopSystem
    {
    public:
        loopSystem() = delete;
        loopSystem(asio::io_context &ioContext, asio::io_context &ioService) : _reg(Registry()), _pos(), _server(ioContext, ioService, _reg)
        {
            _reg.register_component<Position>();
            _reg.register_component<Velocity>();
            _reg.register_component<Hitbox>();
            _reg.register_component<Controllable>();
            _reg.register_component<MovementPattern>();
            _reg.register_component<Destroyable>();
            _reg.register_component<Damaging>();
            _reg.register_component<Score>();
            _reg.register_component<Tag>();
            _reg.spawn_entity(); // Background index
            Entity monster = _reg.spawn_entity();
            _reg.add_component<Position>(monster, {1920, 500});
            _reg.add_component<Velocity>(monster, {2});
            _reg.add_component<MovementPattern>(monster, {STRAIGHTLEFT});
            _reg.add_component<Destroyable>(monster, {2});
            _reg.add_component<Hitbox>(monster, {99, 51});
            _reg.add_component<Damaging>(monster, {true});
            _reg.add_component<Score>(monster, {10});
        };

        ~loopSystem() = default;

        void runNetwork()
        {
            _server.startServer();
        }

        const net::Server &getServerContext() const noexcept { return _server; }
        const Registry &getRegistry() const noexcept { return _reg; }

        void runGame()
        {
            const std::chrono::duration<double> timeInterval(1.0f / 60.0);
            std::chrono::time_point<std::chrono::steady_clock> lastExecutionTime = std::chrono::steady_clock::now();
            while (_server.isSocketOpen())
            {
                if (_server.getClients().size() > 1)
                {
                    std::chrono::time_point<std::chrono::steady_clock> currentTime = std::chrono::steady_clock::now();
                    std::chrono::duration<double> elapsedTime = currentTime - lastExecutionTime;
                    if (elapsedTime >= timeInterval)
                    {
                        currentCooldown--;
                        if (currentCooldown == 0) {
                            _server.sendSparseArray<Position>(packet::ECS_POSITION, _reg.get_components<Position>());
                            _server.sendSparseArray<Velocity>(packet::ECS_VELOCITY, _reg.get_components<Velocity>());
                            _server.sendSparseArray<Hitbox>(packet::ECS_HITBOX, _reg.get_components<Hitbox>());
                            _server.sendSparseArray<Controllable>(packet::ECS_CONTROLLABLE, _reg.get_components<Controllable>());
                            _server.sendSparseArray<Damaging>(packet::ECS_DAMAGES, _reg.get_components<Damaging>());
                            _server.sendSparseArray<Destroyable>(packet::ECS_DESTROYABLE, _reg.get_components<Destroyable>());
                            _server.sendSparseArray<MovementPattern>(packet::ECS_MOVEMENTPATTERN, _reg.get_components<MovementPattern>());
                            _server.sendSparseArray<Score>(packet::ECS_SCORE, _reg.get_components<Score>());
                            _server.sendSparseArray<Tag>(packet::ECS_TAG, _reg.get_components<Tag>());
                            currentCooldown = pingCooldown;
                        }
                        _pos.positionSystemServer(_reg);
                        _dam.damageSystemServer(_reg);

                        lastExecutionTime = currentTime;
                    }
                }
            }
        };

        Registry _reg;
        DamageSystem _dam;
        PositionSystem _pos;
        net::Server _server;
        int pingCooldown = 3;
        int currentCooldown = 3;
    };
}

#endif // R_TYPE_SERVER_HPP
