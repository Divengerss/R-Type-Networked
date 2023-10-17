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

class PositionSystem {
    public:
        PositionSystem() = default;
        ~PositionSystem() = default;
    void positionSystemClient(Registry &r, net::Client &client)
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
            if (positions[i].has_value() && vel && pat)
            {
                if (cont)
                {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                        positions[i].value()._x -= vel.value()._velocity;
                        packet::packetHeader packet(packet::KEYBOARD_EVENT, sizeof(packet::keyboardEvent));
                        packet::keyboardEvent event(client.getUuid(), packet::ACCEPTED, int(sf::Keyboard::Left));
                        client.sendPacket(packet, event);
                    }
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                        positions[i].value()._x += vel.value()._velocity;
                        packet::packetHeader packet(packet::KEYBOARD_EVENT, sizeof(packet::keyboardEvent));
                        packet::keyboardEvent event(client.getUuid(), packet::ACCEPTED, int(sf::Keyboard::Right));
                        client.sendPacket(packet, event);
                    }
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                        positions[i].value()._y -= vel.value()._velocity;
                        packet::packetHeader packet(packet::KEYBOARD_EVENT, sizeof(packet::keyboardEvent));
                        packet::keyboardEvent event(client.getUuid(), packet::ACCEPTED, int(sf::Keyboard::Up));
                        client.sendPacket(packet, event);
                    }
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                        positions[i].value()._y += vel.value()._velocity;
                        packet::packetHeader packet(packet::KEYBOARD_EVENT, sizeof(packet::keyboardEvent));
                        packet::keyboardEvent event(client.getUuid(), packet::ACCEPTED, int(sf::Keyboard::Down));
                        client.sendPacket(packet, event);
                    }
                    if (_spacePressed <= 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                        Entity bullet = r.spawn_entity();
                        r.add_component<Texture>(bullet, {"./Release/assets/sprites/r-typesheet2.gif", 185, 0, 25, 25});
                        r.add_component<Position>(bullet, {positions[i].value()._x + 100, positions[i].value()._y});
                        r.add_component<Scale>(bullet, {3, 3});
                        r.add_component<Velocity>(bullet, {20});
                        r.add_component<MovementPattern>(bullet, {STRAIGHTRIGHT});
                        // r.add_component<Hitbox>(bullet, {25, 25});
                        r.add_component<Damaging>(bullet, 4);
                        _spacePressed = 20;
                        packet::packetHeader packet(packet::KEYBOARD_EVENT, sizeof(packet::keyboardEvent));
                        packet::keyboardEvent event(client.getUuid(), packet::ACCEPTED, int(sf::Keyboard::Space));
                        client.sendPacket(packet, event);
                    }
                    // if (/*sf::Keyboard::isKeyPressed(sf::Keyboard::T)*/entity_a.getSprite().getGlobalBounds().intersects(entity_b.getSprite().getGlobalBounds()))
                    // {
                    //     r.kill_entity(Entity(i));
                    // }
                    _spacePressed--;
                } else {
                    switch (pat.value()._movementPattern)
                    {
                    case (MovementPatterns::STRAIGHTLEFT):
                        positions[i].value()._y = positions[i].value()._y;
                        positions[i].value()._x -= vel.value()._velocity;
                        break;
                    case (MovementPatterns::STRAIGHTRIGHT):
                        positions[i].value()._y = positions[i].value()._y;
                        positions[i].value()._x += vel.value()._velocity;
                        break;
                    case (MovementPatterns::SINUS):
                        positions[i].value()._y = sin(positions[i].value()._x) + pat.value()._baseHeight;
                        break;
                    case (MovementPatterns::CIRCLE):
                        positions[i].value()._y = 0;
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
