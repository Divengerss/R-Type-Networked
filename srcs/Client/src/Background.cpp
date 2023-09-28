/*
** EPITECH PROJECT, 2023
** r-type
** File description:
** Background
*/

#include "Background.hpp"

Background::Background()
{
    _textureSpace.loadFromFile("./Release/assets/Backgrounds/Space.png");
    _Space = sf::Sprite(_textureSpace);
    _SpaceToDisplay = sf::IntRect(0, 0, 950, 200);
    _Space.setTextureRect(_SpaceToDisplay);
    _Space.setScale(3, 3);
}

Background::~Background()
{
}

sf::Sprite Background::getSprite()
{
    return _Space;
}

sf::IntRect Background::getSpaceToDisplay()
{
    return _SpaceToDisplay;
}

void Background::BackgroundScrolling()
{
    _SpaceToDisplay.left += _spaceScrollSpeed;
    if (_SpaceToDisplay.left >= 950)
        _SpaceToDisplay.left = 0;
    _Space.setTextureRect(_SpaceToDisplay);
}