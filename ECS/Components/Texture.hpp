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
        Texture(std::string path) {
            _path = path;
            _texture.loadFromFile(_path);
        };

        std::string _path;
        sf::Texture _texture;
    protected:
    private:
};

#endif /* !TEXTURE_HPP_ */
