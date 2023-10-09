/*
** EPITECH PROJECT, 2023
** r-type
** File description:
** Hitbox
*/

#ifndef HITBOX_HPP_
#define HITBOX_HPP_

class Hitbox {
    public:
        Hitbox(int width, int height) {
            _width = width;
            _height = height;
        };

        int _width;
        int _height;
    protected:
    private:
};

#endif /* !HITBOX_HPP_ */
