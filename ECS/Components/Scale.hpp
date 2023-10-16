/*
** EPITECH PROJECT, 2023
** r-type
** File description:
** Scale
*/

#ifndef SCALE_HPP_
#define SCALE_HPP_

class Scale {
    public:
        Scale(float scaleX, float scaleY) {
            _scaleX = scaleX;
            _scaleY = scaleY;
        };
        ~Scale() = default;

        float _scaleX;
        float _scaleY;
    protected:
    private:
};

#endif /* !SCALE_HPP_ */