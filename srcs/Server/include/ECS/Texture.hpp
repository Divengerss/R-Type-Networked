/*
** EPITECH PROJECT, 2023
** r-type
** File description:
** Texture
*/

#ifndef TEXTURE_HPP_
#define TEXTURE_HPP_

#include <string>

class Texture {
    public:
        Texture(std::string path) {
            _path = path;
        };

        std::string _path;

    protected:
    private:
};

#endif /* !TEXTURE_HPP_ */
