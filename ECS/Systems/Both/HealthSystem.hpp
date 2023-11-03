/*
** EPITECH PROJECT, 2023
** Sans titre(Espace de travail)
** File description:
** HealthSystem
*/

#ifndef HealthSystem_HPP_
#define HealthSystem_HPP_

#include <cstdlib>
#include <iostream>

#include "Registry.hpp"
#include "Tag.hpp"
#include "Texture.hpp"
#include "Position.hpp"
#include "Scale.hpp"
#include "Velocity.hpp"
#include "MovementPattern.hpp"
#include "Destroyable.hpp"
#include "Hitbox.hpp"
#include "Damaging.hpp"
#include "Server/NetworkSystem.hpp"

class HealthSystem
{
public:
    HealthSystem() = default;
    ~HealthSystem() = default;

    void run(Registry &r, rtype::NetworkSystem &networkSystem)
    {
        for (int i = 0; i < r.get_entity_number(); i++)
        {
            if (r.entity_has_component<Destroyable>(Entity(i)))
            {
                auto const &hp = r.get_component<Destroyable>(Entity(i));

                if (hp._hp <= 0)
                {
                    r.kill_entity(Entity(i));
                    networkSystem.handleEntityKilled(Entity(i));
                }
            }
            if (r.entity_has_component<MovementPattern>(Entity(i)) &&
                r.entity_has_component<Position>(Entity(i)) &&
                r.entity_has_component<Tag>(Entity(i)))
            {
                // auto &tag = r.get_component<Tag>(Entity(i));
                auto &pos = r.get_component<Position>(Entity(i));

                if (pos._x > 2000 || pos._x < -100)
                {
                    r.kill_entity(Entity(i));
                    networkSystem.handleEntityKilled(Entity(i));
                }
            }
        }
    }

protected:
private:
};

#endif /* !HealthSystem_HPP_ */
