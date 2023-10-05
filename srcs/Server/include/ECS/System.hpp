/*
** EPITECH PROJECT, 2023
** r-type
** File description:
** System
*/

#ifndef SYSTEM_HPP_
#define SYSTEM_HPP_
#include "Health.hpp"
#include "Hitbox.hpp"
#include "Position.hpp"
#include "Destroyable.hpp"
#include "Damage.hpp"

class System {
    public:
        System();
        ~System();

        void updatePos(int &x, int &y, int velocity) {
            x += velocity;
            y += velocity;
        }

        bool checkDamage(Hitbox HitBox, Position pos, Destroyable destroyable, Damage damage, Health hp) {
            if (HitBox._width == pos._x && HitBox._height == pos._y)
                return true;
            hp._hp -= damage._damage;
            if (hp._hp <= 0)
                return true;
            return false;
        }

        void checkBonus() {
            if (bonus == true)
                return true;
            return false;
        }

    protected:
    private:
};

#endif /* !SYSTEM_HPP_ */