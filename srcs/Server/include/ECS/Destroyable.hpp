/*
** EPITECH PROJECT, 2023
** r-type
** File description:
** Destroyable
*/

#ifndef DESTROYABLE_HPP_
#define DESTROYABLE_HPP_

class Destroyable {
    public:
        Destroyable(bool destroyable) {
            _destroyable = destroyable;
        };

        bool _destroyable;

    protected:
    private:
};

#endif /* !DESTROYABLE_HPP_ */
