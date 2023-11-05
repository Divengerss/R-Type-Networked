/*
** EPITECH PROJECT, 2023
** B-CPP-500-PAR-5-1-rtype-julian.emery
** File description:
** Position_System
*/

#ifndef POSITION_SYSTEM_HPP_
#define POSITION_SYSTEM_HPP_

#include <Registry.hpp>
#include "Position.hpp"
#include "Velocity.hpp"
#include "MovementPattern.hpp"
#include "Controllable.hpp"
#include "Hitbox.hpp"
#include "Damaging.hpp"
#include "Packets.hpp"
#include <cmath>
#include <SDL.h>

class Position_System {
    public:
        Position_System() = default;
        ~Position_System() = default;
    void position_SystemRunner(Registry &r)
    {
        const Uint8* state = SDL_GetKeyboardState(NULL);
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
                // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                // {
                //     pos->_x -= vel->_velocity;
                //     keyPressed = sf::Keyboard::Left;
                // }
                // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                // {
                //     pos->_x += vel->_velocity;
                //     keyPressed = sf::Keyboard::Right;
                // }
                if (state[SDL_SCANCODE_UP])
                {
                    pos->_y -= vel->_velocity;
                    // keyPressed = sf::Keyboard::Up;
                }
                // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                // {
                //     pos->_y += vel->_velocity;
                //     keyPressed = sf::Keyboard::Down;
                // }
                // if (_spacePressed <= 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
                // {
                //     _spacePressed = 20;
                //     keyPressed = sf::Keyboard::Space;
                // }
                // if (keyPressed != sf::Keyboard::Key::Unknown && !sf::Keyboard::isKeyPressed(keyPressed)) {
                //     keyPressed = sf::Keyboard::Key::Unknown;
                // }
                // _spacePressed--;
            }
            else if (pos && vel && pat)
            {
                switch (pat->_movementPattern)
                {
                case (MovementPatterns::STRAIGHTLEFT):
                    pos->_y = pos->_y;
                    pos->_x -= vel->_velocity;
                    break;
                case (MovementPatterns::STRAIGHTRIGHT):
                    pos->_y = pos->_y;
                    pos->_x += vel->_velocity;
                    break;
                case (MovementPatterns::SINUS):
                    pos->_y = sin(pos->_x) + pat->_baseHeight;
                    break;
                case (MovementPatterns::CIRCLE):
                    pos->_y = 0;
                    break;
                case (MovementPatterns::NONE):
                    break;
                }
            }
        }
    }

    protected:
    private:
};

#endif /* !POSITION_SYSTEM_HPP_ */
