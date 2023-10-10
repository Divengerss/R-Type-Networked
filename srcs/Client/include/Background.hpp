/*
** EPITECH PROJECT, 2023
** r-type
** File description:
** Background
*/

#ifndef BACKGROUND_HPP_
#define BACKGROUND_HPP_

#include <SFML/Graphics.hpp>

class Background {
    public:
        Background();
        ~Background();

        sf::Sprite getSprite();
        sf::IntRect getSpaceToDisplay();
        void BackgroundScrolling();
    protected:
    private:
        sf::Texture _textureSpace;
        sf::Sprite _Space;
        sf::IntRect _SpaceToDisplay;
        int _spaceScrollSpeed = 1;
};

#endif /* !BACKGROUND_HPP_ */
