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
#include "Pickup.hpp"
#include "ActiveBonus.hpp"

class System {
    public:
        System();
        ~System();

        void updatePos(int &x, int &y, int velocity) {
            x += velocity;
            y += velocity;
        }

        bool checkDamage(Hitbox HitBox_a, Hitbox HitBox_b, Position pos_a, Position pos_b, Destroyable destroyable, Damage damage, Health hp) {
            int point_right_corner_a = pos_a._x + HitBox_a._width;
            int point_right_corner_b = pos_b._x + HitBox_b._width;

            if (pos_b._x > pos_a._x && pos_b._x < point_right_corner_a || point_right_corner_b > pos_a._x && point_right_corner_b < point_right_corner_a
            && pos_b._y > pos_a._y && pos_b._y < pos_a._y + HitBox_a._height || pos_b._y + HitBox_b._height > pos_a._y && pos_b._y + HitBox_b._height < pos_a._y + HitBox_a._height) {
                if (destroyable._destroyable == true && damage._damage == true && hp._hp > 0)
                    return true;
                return true;
            }
            return false;
        }

        bool checkBonus(Hitbox HitBox_p, Hitbox HitBox_b, Position pos_p, Position pos_b, Pickup pickup, ActiveBonus activeBonus) {
            int point_right_corner_p = pos_p._x + HitBox_p._width;
            int point_right_corner_b = pos_b._x + HitBox_b._width;

            if (pos_b._x > pos_p._x && pos_b._x < point_right_corner_p || point_right_corner_b > pos_p._x && point_right_corner_b < point_right_corner_p
            && pos_b._y > pos_p._y && pos_b._y < pos_p._y + HitBox_p._height || pos_b._y + HitBox_b._height > pos_p._y && pos_b._y + HitBox_b._height < pos_p._y + HitBox_p._height) {
                if (pickup._pickup == true) // mettre active bonus aléatoire ?
                    return true;
            }
            return false;
        }

    protected:
    private:
};

#endif /* !SYSTEM_HPP_ */