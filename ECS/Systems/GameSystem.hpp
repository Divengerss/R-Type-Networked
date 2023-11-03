/*
** EPITECH PROJECT, 2023
** Sans titre(Espace de travail)
** File description:
** GameSystem
*/

#ifndef GAMESYSTEM_HPP_
#define GAMESYSTEM_HPP_

#include "Registry.hpp"

#include "Destroyable.hpp"
#include "Position.hpp"
#include "Controllable.hpp"
#include "Velocity.hpp"
#include "Hitbox.hpp"
#include "Damaging.hpp"
#include "MovementPattern.hpp"

class GameSystem
{
public:
    void gameSystem(Registry &r)
    {
        auto hps = r.get_components<Destroyable>();
        auto positions = r.get_components<Position>();
        auto controllables = r.get_components<Controllable>();
        for (size_t i = 0; i < hps.size(); ++i) {
            auto cont = controllables[i];
            auto hp = hps[i];
            if (hp && !cont && hp.value()._hp <= 0)
                enemiesToKill--;
        }
        if (enemiesToKill <= 0) {
            currentLevel++;
            enemiesToKill = currentLevel * 10;
            enemySpawnAtOnce++;
            if (enemySpawnCooldown >= 0.2)
                enemySpawnCooldown -= 0.1;
        }
        currentCooldown -= 0.1;
        if (currentCooldown <= 0)
            currentCooldown = enemySpawnCooldown;
            //ENEMY CREATION
            auto entity = r.spawn_entity();
            r.add_component<Position>(entity, {2000, 500});
            r.add_component<Velocity>(entity, 1);
            r.add_component<Hitbox>(entity, {100, 100});
            r.add_component<Destroyable>(entity, 1);
            r.add_component<Damaging>(entity, 1);
            r.add_component<MovementPattern>(entity, MovementPatterns::SINUS);
    }
    int currentLevel = 1;
    int enemiesToKill = 10;
    float enemySpawnCooldown = 1;
    float enemySpawnAtOnce = 1;
    float currentCooldown = 1;

protected:
private:
};

#endif /* !GAMESYSTEM_HPP_ */
