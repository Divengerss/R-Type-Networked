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
        Destroyable(int hp) {
            _hp = hp;
            _invincible = false;
            _invincibleTimeInSec = 3;
            _invincibleTime = 0;
        };

        int _hp;
        bool _invincible;
        float _invincibleTimeInSec;
        float _invincibleTime;
};

#endif /* !DESTROYABLE_HPP_ */
