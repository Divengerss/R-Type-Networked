/*
** EPITECH PROJECT, 2023
** r-type
** File description:
** Sprite.hpp
*/

#ifndef SPRITE_HPP_
#define SPRITE_HPP_

#include <string>
#include <SFML/Graphics.hpp>

class Sprite
{
public:
    Sprite(const sf::Texture &texture)
    {
        _sprite.setTexture(texture);
    }

    sf::Sprite _sprite;

protected:
private:
};

#endif /* !SPRITE_HPP_ */
