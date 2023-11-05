/*
** EPITECH PROJECT, 2023
** r-type
** File description:
** HealthBar.hpp
*/

#ifndef HEALTHBAR_HPP_
#define HEALTHBAR_HPP_

#include <string>
#include <SFML/Graphics.hpp>

class HealthBar
{
public:
    HealthBar(float width, float height, int initialHealth, sf::Color color)
    {
        _width = width;
        _initialHealth = initialHealth;
        _healthBar.setSize(sf::Vector2f(width, height));
        _healthBar.setFillColor(color);
    }

    void update(int health, float posX, float posY)
    {
        _healthBar.setPosition(posX, posY);
        _healthBar.setSize(sf::Vector2f(_width * health / _initialHealth, _healthBar.getSize().y));
    }

    void draw(sf::RenderWindow &window)
    {
        window.draw(_healthBar);
    }


protected:
private:
    sf::RectangleShape _healthBar;
    float _width;
    int _initialHealth;
};

#endif /* !HEALTHBAR_HPP_ */
