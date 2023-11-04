/*
** EPITECH PROJECT, 2023
** Sans titre(Espace de travail)
** File description:
** PositionSystem
*/

#ifndef POSITIONSYSTEM_HPP_
#define POSITIONSYSTEM_HPP_

#include <Registry.hpp>
#include "Position.hpp"
#include "Velocity.hpp"
#include "MovementPattern.hpp"
#include "Controllable.hpp"
#include "Hitbox.hpp"
#include "Damaging.hpp"
#include "Packets.hpp"
#include "Tag.hpp"
#include <cmath>

class PositionSystem
{
public:
    PositionSystem() = default;
    ~PositionSystem() = default;

    void positionSystemServer(Registry &r)
    {
        for (int i = 0; i < r.get_entity_number(); i++)
        {
            if (r.entity_has_component<Position>(Entity(i)) && r.entity_has_component<Velocity>(Entity(i)))
            {
                auto &pos = r.get_component<Position>(Entity(i));
                auto &vel = r.get_component<Velocity>(Entity(i));

                if (r.entity_has_component<Controllable>(Entity(i)))
                {
                    auto &cont = r.get_component<Controllable>(Entity(i));

                    if (cont.latestInput == 71)
                    {
                        if (pos._x > 10)
                            pos._x -= vel._velocity;
                    }
                    if (cont.latestInput == 72)
                    {
                        if (pos._x < 1900)
                            pos._x += vel._velocity;
                    }
                    if (cont.latestInput == 73)
                    {
                        if (pos._y > 10)
                            pos._y -= vel._velocity;
                    }
                    if (cont.latestInput == 74)
                    {
                        if (pos._y < 1050)
                            pos._y += vel._velocity;
                    }
                    if (_spacePressed <= 0 && cont.latestInput == 57)
                    {
                        Entity bullet = r.spawn_entity();
                        r.add_component<Position>(bullet, {pos._x + 100, pos._y});
                        r.add_component<Velocity>(bullet, {20});
                        r.add_component<MovementPattern>(bullet, {STRAIGHTRIGHT});
                        r.add_component<Damaging>(bullet, 4);
                        r.add_component<Tag>(bullet, {TagEnum::BULLET});
                        r.add_component<Destroyable>(bullet, {1});
                        r.add_component<Hitbox>(bullet, {25, 25});
                        _spacePressed = 20;
                    }
                    _spacePressed--;
                }
                else if (r.entity_has_component<MovementPattern>(Entity(i)))
                {
                    auto &pat = r.get_component<MovementPattern>(Entity(i));

                    switch (pat._movementPattern)
                    {
                    case (MovementPatterns::STRAIGHTLEFT):
                        pos._y = pos._y;
                        pos._x -= vel._velocity;
                        break;
                    case (MovementPatterns::STRAIGHTRIGHT):
                        pos._y = pos._y;
                        pos._x += vel._velocity;
                        break;
                    case (MovementPatterns::SINUS):
                        pos._y = sin(pos._x) + pat._baseHeight;
                        break;
                    case (MovementPatterns::CIRCLE):
                        pos._y = 0;
                        break;
                    case (MovementPatterns::NONE):
                        break;
                    }
                }
            }
        }
    }

    int _spacePressed = 200;

protected:
private:
};

#endif /* !POSITIONSYSTEM_HPP_ */
