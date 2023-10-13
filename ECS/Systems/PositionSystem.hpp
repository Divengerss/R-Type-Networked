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
#include <cmath>

class PositionSystem {
    public:
        PositionSystem() = default;
        ~PositionSystem() = default;
    void positionSystem(Registry &r)
    {
        auto &positions = r.get_components<Position>();
        auto &velocities = r.get_components<Velocity>();
        auto &patterns = r.get_components<MovementPattern>();
        auto &controllables = r.get_components<Controllable>();
        for (size_t i = 0; i < positions.size(); ++i)
        {
            //auto &pos = positions[i];
            auto const &vel = velocities[i];
            auto const &pat = patterns[i];
            auto const &cont = controllables[i];
            std::cout << i << std::endl;
            if (positions[i].has_value() && vel && pat)
            {
                if (cont->_playable == true)
                {
                    std::cout << "cont" << std::endl;
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                        positions[i].value()._x -= vel.value()._velocity;
                    }
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                        positions[i].value()._x += vel.value()._velocity;
                    }
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                        positions[i].value()._y -= vel.value()._velocity;
                    }
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                        positions[i].value()._y += vel.value()._velocity;
                    }
                    //PLAYER MOVEMENT
                } else {
                    std::cout << "not cont" << std::endl;
                    switch (pat.value()._movementPattern)
                    {
                    case (MovementPatterns::STRAIGHTLEFT):
                        positions[i].value()._y = positions[i].value()._y;
                        positions[i].value()._x -= vel.value()._velocity;
                        // if (positions[i].value()._x <= -1920)
                        //     positions[i].value()._x = 1920;
                        break;
                    case (MovementPatterns::SINUS):
                        positions[i].value()._y = sin(positions[i].value()._x) + pat.value()._baseHeight;
                        break;
                    case (MovementPatterns::CIRCLE):
                        positions[i].value()._y = 0;
                        break;
                    }
                }
            }
        }
    }

    protected:
    private:
};

#endif /* !POSITIONSYSTEM_HPP_ */
