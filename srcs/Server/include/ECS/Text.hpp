/*
** EPITECH PROJECT, 2023
** r-type
** File description:
** Text
*/

#ifndef TEXT_HPP_
#define TEXT_HPP_

#include <string>
#include <SFML/Graphics.hpp>

class Text {
    public:
        Text(std::string text, int size = 1, sf::Color color = sf::Color::White) {
            _text = text;
            _size = size;
            _color = color;
        };
        std::string _text;
        int _size;
        sf::Color _color;

    protected:
    private:
};

#endif /* !TEXT_HPP_ */
