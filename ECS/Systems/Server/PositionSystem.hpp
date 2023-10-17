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
#include <cmath>

class PositionSystem
{
public:
    PositionSystem() = default;
    ~PositionSystem() = default;

    void positionSystemServer(Registry &r)
    {
        auto &positions = r.get_components<Position>();
        auto &velocities = r.get_components<Velocity>();
        auto &patterns = r.get_components<MovementPattern>();
        auto &controllables = r.get_components<Controllable>();
        for (size_t i = 0; i < positions.size(); ++i)
        {
            auto &pos = positions[i];
            auto const &vel = velocities[i];
            auto const &pat = patterns[i];
            auto const &cont = controllables[i];
            if (pos && vel && cont)
            {
                if (cont->latestInput == 71)
                {
                    pos->_x -= vel.value()._velocity;
                }
                if (cont->latestInput == 72)
                {
                    pos->_x += vel.value()._velocity;
                }
                if (cont->latestInput == 73)
                {
                    pos->_y -= vel.value()._velocity;
                }
                if (cont->latestInput == 74)
                {
                    pos->_y += vel.value()._velocity;
                }
                if (_spacePressed <= 0 && cont->latestInput == 57)
                {
                    Entity bullet = r.spawn_entity();
                    r.add_component<Position>(bullet, {pos->_x + 100, pos->_y});
                    r.add_component<Velocity>(bullet, {20});
                    r.add_component<MovementPattern>(bullet, {STRAIGHTRIGHT});
                    r.add_component<Damaging>(bullet, 4);
                    _spacePressed = 20;
                }
                _spacePressed--;
            }
            else if (pos && vel && pat)
            {
                switch (pat.value()._movementPattern)
                {
                case (MovementPatterns::STRAIGHTLEFT):
                    pos->_y = pos->_y;
                    pos->_x -= vel.value()._velocity;
                    break;
                case (MovementPatterns::STRAIGHTRIGHT):
                    pos->_y = pos->_y;
                    pos->_x += vel.value()._velocity;
                    break;
                case (MovementPatterns::SINUS):
                    pos->_y = sin(pos->_x) + pat.value()._baseHeight;
                    break;
                case (MovementPatterns::CIRCLE):
                    pos->_y = 0;
                    break;
                case (MovementPatterns::NONE):
                    break;
                }
                if (pos->_x > 2000 || pos->_x < 0) {
                    r.kill_entity(Entity(i));
                }
            }
        }
    }

    int _spacePressed = 200;

protected:
private:
};

#endif /* !POSITIONSYSTEM_HPP_ */
