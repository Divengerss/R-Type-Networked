/*
** EPITECH PROJECT, 2023
** r-type
** File description:
** Player
*/

#include "Player.hpp"

#include <iostream>

Player::Player()
{
    _texture.loadFromFile("./Release/assets/sprites/ship.gif");
    _Ship = sf::Sprite(_texture); // pas sûr
    _ShipToDisplay = sf::IntRect(66, 51, 33, 17);
    _Ship.setTextureRect(_ShipToDisplay);
    _Ship.setScale(2, 2);
}

Player::~Player()
{
}

void Player::Movement()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        _Ship.setPosition(_Ship.getPosition().x - 1, _Ship.getPosition().y);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        _Ship.setPosition(_Ship.getPosition().x + 1, _Ship.getPosition().y);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        _Ship.setPosition(_Ship.getPosition().x, _Ship.getPosition().y - 1);
        _ShipToDisplay.left += 33;
        if (_ShipToDisplay.left >= 132)
            _ShipToDisplay.left = 132;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        _Ship.setPosition(_Ship.getPosition().x, _Ship.getPosition().y + 1);
        _ShipToDisplay.left -= 33;
        if (_ShipToDisplay.left <= 0)
            _ShipToDisplay.left = 0;
    }
    _Ship.setTextureRect(_ShipToDisplay);
    _ShipToDisplay.left = 66;
}

sf::Sprite Player::getSprite()
{
    return _Ship;
}

// void ResetShipToDisplay()
// {
//     _ShipToDisplay.left = 66.4;
// } une erreur mais pas checké

sf::IntRect Player::getShipToDisplay()
{
    return _ShipToDisplay;
}