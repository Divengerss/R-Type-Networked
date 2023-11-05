/*
** EPITECH PROJECT, 2023
** Sans titre(Espace de travail)
** File description:
** InputSystem
*/

#ifndef INPUTSYSTEM_HPP_
#define INPUTSYSTEM_HPP_

#include "Registry.hpp"
#include "Network.hpp"
#include <SFML/Graphics.hpp>

class InputSystem
{
public:
    void inputSystem(Registry &r, net::Client &client)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            packet::packetHeader packet(packet::KEYBOARD_EVENT, sizeof(packet::keyboardEvent));
            packet::keyboardEvent event(client.getUuid(), packet::ACCEPTED, int(sf::Keyboard::Left), client.getRoomId());
            client.sendPacket(packet, event);
            keyPressed = sf::Keyboard::Left;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            packet::packetHeader packet(packet::KEYBOARD_EVENT, sizeof(packet::keyboardEvent));
            packet::keyboardEvent event(client.getUuid(), packet::ACCEPTED, int(sf::Keyboard::Right), client.getRoomId());
            client.sendPacket(packet, event);
            keyPressed = sf::Keyboard::Right;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            packet::packetHeader packet(packet::KEYBOARD_EVENT, sizeof(packet::keyboardEvent));
            packet::keyboardEvent event(client.getUuid(), packet::ACCEPTED, int(sf::Keyboard::Up), client.getRoomId());
            client.sendPacket(packet, event);
            keyPressed = sf::Keyboard::Up;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            packet::packetHeader packet(packet::KEYBOARD_EVENT, sizeof(packet::keyboardEvent));
            packet::keyboardEvent event(client.getUuid(), packet::ACCEPTED, int(sf::Keyboard::Down), client.getRoomId());
            client.sendPacket(packet, event);
            keyPressed = sf::Keyboard::Down;
        }
        if (_spacePressed <= 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            _spacePressed = 20;
            packet::packetHeader packet(packet::KEYBOARD_EVENT, sizeof(packet::keyboardEvent));
            packet::keyboardEvent event(client.getUuid(), packet::ACCEPTED, int(sf::Keyboard::Space), client.getRoomId());
            client.sendPacket(packet, event);
            keyPressed = sf::Keyboard::Space;
        }
        if (keyPressed != sf::Keyboard::Key::Unknown && !sf::Keyboard::isKeyPressed(keyPressed))
        {
            packet::packetHeader packet(packet::KEYBOARD_EVENT, sizeof(packet::keyboardEvent));
            packet::keyboardEvent event(client.getUuid(), packet::ACCEPTED, int(sf::Keyboard::Unknown), client.getRoomId());
            client.sendPacket(packet, event);
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