/*
** EPITECH PROJECT, 2023
** Sans titre(Espace de travail)
** File description:
** InputSystem
*/

#ifndef INPUTSYSTEM_HPP_
#define INPUTSYSTEM_HPP_

#include "ISystem.hpp"
#include "Registry.hpp"
#include <SFML/Graphics.hpp>

class InputSystem : public ISystem
{
public:
    void runSystem(Registry &r)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            packet::packetHeader packet(packet::KEYBOARD_EVENT, sizeof(packet::keyboardEvent));
            packet::keyboardEvent event("", packet::ACCEPTED, int(sf::Keyboard::Left));
            r.queueToSend.push({packet, event});
            keyPressed = sf::Keyboard::Left;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            packet::packetHeader packet(packet::KEYBOARD_EVENT, sizeof(packet::keyboardEvent));
            packet::keyboardEvent event("", packet::ACCEPTED, int(sf::Keyboard::Right));
            r.queueToSend.push({packet, event});
            keyPressed = sf::Keyboard::Right;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            packet::packetHeader packet(packet::KEYBOARD_EVENT, sizeof(packet::keyboardEvent));
            packet::keyboardEvent event("", packet::ACCEPTED, int(sf::Keyboard::Up));
            r.queueToSend.push({packet, event});
            keyPressed = sf::Keyboard::Up;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            packet::packetHeader packet(packet::KEYBOARD_EVENT, sizeof(packet::keyboardEvent));
            packet::keyboardEvent event("", packet::ACCEPTED, int(sf::Keyboard::Down));
            r.queueToSend.push({packet, event});
            keyPressed = sf::Keyboard::Down;
        }
        if (_spacePressed <= 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            _spacePressed = 20;
            packet::packetHeader packet(packet::KEYBOARD_EVENT, sizeof(packet::keyboardEvent));
            packet::keyboardEvent event("", packet::ACCEPTED, int(sf::Keyboard::Space));
            r.queueToSend.push({packet, event});
            keyPressed = sf::Keyboard::Space;
        }
        if (keyPressed != sf::Keyboard::Key::Unknown && !sf::Keyboard::isKeyPressed(keyPressed))
        {
            packet::packetHeader packet(packet::KEYBOARD_EVENT, sizeof(packet::keyboardEvent));
            packet::keyboardEvent event("", packet::ACCEPTED, int(sf::Keyboard::Unknown));
            r.queueToSend.push({packet, event});
            keyPressed = sf::Keyboard::Key::Unknown;
        }
        _spacePressed--;
    }

    int _spacePressed = 200;
    sf::Keyboard::Key keyPressed = sf::Keyboard::Key::Unknown;

protected:
private:
};

#endif /* !INPUTSYSTEM_HPP_ */
