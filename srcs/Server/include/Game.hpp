#ifndef R_TYPE_SERVER_HPP
#define R_TYPE_SERVER_HPP

#ifdef _WIN32
#define _WIN32_WINNT 0x0601
#endif /* !_WIN32 */

#include "Network.hpp"
#include "Position.hpp"
#include "Velocity.hpp"
#include "Hitbox.hpp"
#include "Damaging.hpp"
#include "Destroyable.hpp"
#include "Controllable.hpp"
#include "Entity.hpp"
#include "Server/PositionSystem.hpp"
#include "Server/DamageSystem.hpp"
#include "Server/NetworkSystem.hpp"
#include "Server/WaveSystem.hpp"
#include "Both/HealthSystem.hpp"
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
        loopSystem() : _reg(Registry()), _pos(), _netSys(),
                       _pingCooldown(3), _currentCooldown(3)
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
            _reg.spawn_entity(); // ChatBox index
        };

        ~loopSystem()
        {
            for (auto &thread : _threadPool)
                if (thread.joinable())
                    thread.join();
        };

        void runNetwork()
        {
            _threadPool.emplace_back([&]()
                                     { _netSys.networkSystemServer(_reg); });
        }

        const Registry &getRegistry() const noexcept { return _reg; }

        void runGame()
        {
            const std::chrono::duration<double> timeInterval(1.0f / 60.0);
            std::chrono::time_point<std::chrono::steady_clock> lastExecutionTime = std::chrono::steady_clock::now();
            std::chrono::time_point<std::chrono::steady_clock> currentTime;
            std::chrono::duration<double> elapsedTime;

            while (_netSys.isServerAvailable())
            {
                if (_netSys.getConnectedNb() > 1)
                {
                    currentTime = std::chrono::steady_clock::now();
                    elapsedTime = currentTime - lastExecutionTime;
                    if (elapsedTime >= timeInterval)
                    {

                        _currentCooldown--;
                        if (_currentCooldown == 0)
                        {
                            _netSys.sendSparseArray<Position>(packet::ECS_POSITION, _reg.get_components<Position>());
                            _netSys.sendSparseArray<Velocity>(packet::ECS_VELOCITY, _reg.get_components<Velocity>());
                            _netSys.sendSparseArray<Hitbox>(packet::ECS_HITBOX, _reg.get_components<Hitbox>());
                            _netSys.sendSparseArray<Controllable>(packet::ECS_CONTROLLABLE, _reg.get_components<Controllable>());
                            _netSys.sendSparseArray<Damaging>(packet::ECS_DAMAGES, _reg.get_components<Damaging>());
                            _netSys.sendSparseArray<Destroyable>(packet::ECS_DESTROYABLE, _reg.get_components<Destroyable>());
                            _netSys.sendSparseArray<MovementPattern>(packet::ECS_MOVEMENTPATTERN, _reg.get_components<MovementPattern>());
                            _netSys.sendSparseArray<Score>(packet::ECS_SCORE, _reg.get_components<Score>());
                            _netSys.sendSparseArray<Tag>(packet::ECS_TAG, _reg.get_components<Tag>());
                            _currentCooldown = _pingCooldown;
                        }
                        _pos.positionSystemServer(_reg);
                        _dam.damageSystemServer(_reg);
                        _waveSystem.run(_reg);
                        _healthSystem.run(_reg, _netSys);
                        lastExecutionTime = currentTime;
                    }
                }
            }
        };

        Registry _reg;
        DamageSystem _dam;
        PositionSystem _pos;
        NetworkSystem _netSys;
        WaveSystem _waveSystem;
        HealthSystem _healthSystem;
        std::vector<std::thread> _threadPool;
        int _pingCooldown;
        int _currentCooldown;
    };
}

#endif // R_TYPE_SERVER_HPP
