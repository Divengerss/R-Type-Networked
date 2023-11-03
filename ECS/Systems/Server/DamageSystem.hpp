/*
** EPITECH PROJECT, 2023
** Sans titre(Espace de travail)
** File description:
** DamageSystem
*/

#ifndef DAMAGESYSTEM_HPP_
#define DAMAGESYSTEM_HPP_

#include "Registry.hpp"
#include "Hitbox.hpp"
#include "Position.hpp"
#include "Damaging.hpp"
#include "Destroyable.hpp"

class DamageSystem
{
public:
    DamageSystem() = default;
    ~DamageSystem() = default;

    void damageSystemServer(Registry &r)
    {
        for (int i = 0; i < r.get_entity_number(); i++)
        {
            if (!r.entity_has_component<Tag>(Entity(i)) ||
                !r.entity_has_component<Destroyable>(Entity(i)) ||
                !r.entity_has_component<Position>(Entity(i)) ||
                !r.entity_has_component<Hitbox>(Entity(i)))
                continue;

            auto &a_hp = r.get_component<Destroyable>(Entity(i));
            auto &a_pos = r.get_component<Position>(Entity(i));
            auto &a_hb = r.get_component<Hitbox>(Entity(i));
            auto &a_tag = r.get_component<Tag>(Entity(i))._tag;

            for (int j = 0; j < r.get_entity_number(); j++)
            {
                if (i == j)
                    continue;

                if (!r.entity_has_component<Tag>(Entity(j)) ||
                    !r.entity_has_component<Destroyable>(Entity(j)) ||
                    !r.entity_has_component<Position>(Entity(j)) ||
                    !r.entity_has_component<Hitbox>(Entity(j)))
                    continue;

                auto &b_hp = r.get_component<Destroyable>(Entity(j));
                auto &b_pos = r.get_component<Position>(Entity(j));
                auto &b_hb = r.get_component<Hitbox>(Entity(j));
                auto &b_tag = r.get_component<Tag>(Entity(j))._tag;

                // hitbox 1
                float damLeft = a_pos._x;
                float damTop = a_pos._y;
                float damRight = damLeft + a_hb._width;
                float damBottom = damTop + a_hb._height;

                // hitbox 2
                float destLeft = b_pos._x;
                float destTop = b_pos._y;
                float destRight = destLeft + b_hb._width;
                float destBottom = destTop + b_hb._height;

                if (damLeft > destRight || damRight < destLeft ||
                    damTop > destBottom || damBottom < destTop)
                    continue;

                if (a_tag == TagEnum::PLAYER && b_tag == TagEnum::ENEMY)
                {
                    a_hp._hp = 0;
                }
                else if (a_tag == TagEnum::BULLET && b_tag == TagEnum::ENEMY)
                {
                    a_hp._hp = 0;
                    b_hp._hp = 0;
                }
            }
        }
    }

protected:
private:
};

#endif /* !DAMAGESYSTEM_HPP_ */
