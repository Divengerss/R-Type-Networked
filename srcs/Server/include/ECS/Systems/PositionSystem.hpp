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
#include <cmath>

class PositionSystem {
    public:
        PositionSystem();
        ~PositionSystem();

    void positionSystem(Registry &r)
    {
        auto positions = r.get_components<Position>();
        auto const velocities = r.get_components<Velocity>();
        auto const patterns = r.get_components<MovementPattern>();

        for (size_t i = 0; i < positions.size(); ++i)
        {
            auto &pos = positions[i];
            auto const &vel = velocities[i];
            auto const &pat = patterns[i];

            if (pos && vel && pat)
            {
                pos.value()._x = pos.value()._x + vel.value()._velocity;
                switch (pat.value()._movementPattern)
                {
                case (MovementPatterns::STRAIGHT):
                    pos.value()._y = pos.value()._y;
                case (MovementPatterns::SINUS):
                    pos.value()._y = sin(pos.value()._x) + pat.value()._baseHeight;
                case (MovementPatterns::CIRCLE):
                    pos.value()._y = 0;
                }
            }
        }
    }

    protected:
    private:
};

#endif /* !POSITIONSYSTEM_HPP_ */
