/*
** EPITECH PROJECT, 2023
** r-type
** File description:
** Player
*/

#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include <SFML/Graphics.hpp>

class Player {
    public:
        Player();
        ~Player();

        void Movement();
        sf::Sprite getSprite();
        // void ResetShipToDisplay(sf::IntRect ShipToDisplay); Une erreur mais pas checké
        sf::IntRect getShipToDisplay();
    protected:
    private:
        sf::Texture _texture;
        sf::Sprite _Ship;
        sf::IntRect _ShipToDisplay;
};

#endif /* !PLAYER_HPP_ */
