/*
** EPITECH PROJECT, 2023
** r-type
** File description:
** Texture
*/

#ifndef TEXTURE_HPP_
#define TEXTURE_HPP_

#include <string>
#include <SFML/Graphics.hpp>

class Texture {
    public:
        Texture(std::string path, int left, int top, int width, int height) {
            _path = path;
            _texture.loadFromFile(_path);
            _left = left;
            _top = top;
            _width = width;
            _height = height;
        };

        std::string _path;
        sf::Texture _texture;
        int _left;
        int _top;
        int _width;
        int _height;
    protected:
    private:
};

#endif /* !TEXTURE_HPP_ */
