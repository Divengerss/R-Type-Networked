/*
** EPITECH PROJECT, 2023
** r-type
** File description:
** Text
*/

#ifndef TEXT_HPP_
#define TEXT_HPP_

#include <string>

class Text {
    public:
        Text(const std::string &text, int size = 1) {
            _text = text;
            _size = size;
        };

        std::string _text;
        int _size;
};

#endif /* !TEXT_HPP_ */
