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
#include "Server/HealthSystem.hpp"
#include "ColliderSystem.hpp"
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
        loopSystem() :
            _regs({}), _pos(), _netSys(), _currentCooldown(3)
        {
        };

        ~loopSystem()
        {
            for (auto &thread : _threadPool)
                if (thread.joinable())
                    thread.join();
        };

        void runNetwork()
        {
            _threadPool.emplace_back([&](){
                _netSys.networkSystemServer(_regs);
            });
        }

        void runGame() {
            const std::chrono::duration<double> frameTimeInterval(1.0f / 60.0f);
            const std::chrono::duration<double> sendSparseArrayInterval(280 * frameTimeInterval.count());
            std::chrono::time_point<std::chrono::steady_clock> lastFrameExecutionTime = std::chrono::steady_clock::now();
            std::chrono::time_point<std::chrono::steady_clock> lastPrintTime = std::chrono::steady_clock::now();
            int frameCounter = 0;
            int printMessageCounter = 0;
            std::chrono::high_resolution_clock::time_point secondCounter = std::chrono::high_resolution_clock::now();

            while (_netSys.isServerAvailable()) {
                std::chrono::time_point<std::chrono::steady_clock> currentTime = std::chrono::steady_clock::now();
                std::chrono::duration<double> elapsedTime = currentTime - lastFrameExecutionTime;
                for (auto &[roomId, reg] : _regs) {
                    if (_netSys.getRooms()[roomId].getClients().size() > 1UL) {
                        frameCounter++;
                        if (elapsedTime >= frameTimeInterval) {
                            _currentCooldown++;
                            if (_currentCooldown % static_cast<int>(sendSparseArrayInterval.count()) == 0) {
                                _netSys.sendSparseArray<Position>(packet::ECS_POSITION, reg.get_components<Position>(), roomId);
                                _netSys.sendSparseArray<Velocity>(packet::ECS_VELOCITY, reg.get_components<Velocity>(), roomId);
                                // _netSys.sendSparseArray<Collider>(packet::ECS_COLLIDER, reg.get_components<Collider>(), roomId);
                                _netSys.sendSparseArray<Hitbox>(packet::ECS_HITBOX, reg.get_components<Hitbox>(), roomId);
                                _netSys.sendSparseArray<Controllable>(packet::ECS_CONTROLLABLE, reg.get_components<Controllable>(), roomId);
                                _netSys.sendSparseArray<Damaging>(packet::ECS_DAMAGES, reg.get_components<Damaging>(), roomId);
                                _netSys.sendSparseArray<Destroyable>(packet::ECS_DESTROYABLE, reg.get_components<Destroyable>(), roomId);
                                _netSys.sendSparseArray<MovementPattern>(packet::ECS_MOVEMENTPATTERN, reg.get_components<MovementPattern>(), roomId);
                                _netSys.sendSparseArray<Score>(packet::ECS_SCORE, reg.get_components<Score>(), roomId);
                                _netSys.sendSparseArray<Tag>(packet::ECS_TAG, reg.get_components<Tag>(), roomId);
                                _currentCooldown = 0;
                            }
                            for (int i = 0; i < reg.get_entity_number(); i++) {
                                if (reg.entity_has_component<Destroyable>(Entity(i))) {
                                    auto &destroyable = reg.get_component<Destroyable>(Entity(i));

                                    if (destroyable._invincible && std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - secondCounter).count() >= 1) {
                                        destroyable._invincibleTime += 1;
                                        secondCounter = std::chrono::high_resolution_clock::now();
                                    }
                                }
                            }
                            // _col.colliderSystem(reg);
                            _pos.positionSystemServer(reg);
                            _dam.damageSystemServer(reg);
                            _waveSystem.run(reg);
                            _healthSystem.run(roomId, reg, _netSys);
                            lastFrameExecutionTime = currentTime;
                        }
                    }
                }
                std::chrono::duration<double> timeSinceLastPrint = currentTime - lastPrintTime;
                printMessageCounter++;
                if (timeSinceLastPrint >= std::chrono::duration<double>(5.0f)) {
                    for (auto &[roomId, reg] : _regs) {
                        while (_netSys.removeTimeoutClients(roomId, reg)) {}
                    }
                    lastPrintTime = currentTime;
                }
            }
        }

        std::unordered_map<std::uint64_t, Registry> _regs; // roomId | ECS
        ColliderSystem _col;
        DamageSystem _dam;
        PositionSystem _pos;
        NetworkSystem _netSys;
        WaveSystem _waveSystem;
        HealthSystem _healthSystem;
        std::vector<std::thread> _threadPool;
        int _currentCooldown;
    };
}

#endif // R_TYPE_SERVER_HPP
