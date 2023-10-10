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
        Text(std::string text, int size = 1) {
            _text = text;
            _size = size;
        };
        std::string _text;
        int _size;

    protected:
    private:
};

#endif /* !TEXT_HPP_ */
