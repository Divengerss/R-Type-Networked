/*
** EPITECH PROJECT, 2023
** Sans titre(Espace de travail)
** File description:
** CollidingSystem
*/

#ifndef COLLIDINGSYSTEM_HPP_
#define COLLIDINGSYSTEM_HPP_

#include "Collider.hpp"
#include "Position.hpp"
#include "Hitbox.hpp"
#include "Collider.hpp"

#include "Registry.hpp"

class ColliderSystem
{
public:
    void colliderSystem(Registry &r)
    {
        for (size_t i = 0; i < r.get_entity_number(); i++)
        {
            if (!r.entity_has_component<Hitbox>(Entity(i)) ||
                !r.entity_has_component<Position>(Entity(i)))
                continue;

            auto const &hb_fir = r.get_component<Hitbox>(Entity(i));
            auto const &pos_fir = r.get_component<Position>(Entity(i));
            auto &col_fir = r.get_components<Collider>()[i];
            for (size_t j = 0; j < r.get_entity_number(); ++j)
            {
                if (i == j)
                    continue;
                if (!r.entity_has_component<Hitbox>(Entity(j)) ||
                    !r.entity_has_component<Position>(Entity(j)))
                    continue;

                auto const &pos_sec = r.get_component<Position>(Entity(j));
                auto const &hb_sec = r.get_component<Hitbox>(Entity(j));
                auto &col_sec = r.get_components<Collider>()[j];

                if (
                    pos_fir._x + hb_fir._width >= pos_sec._x &&
                    pos_fir._x <= pos_sec._x + hb_sec._width &&
                    pos_fir._y + hb_fir._height >= pos_sec._y &&
                    pos_fir._y <= pos_sec._y + hb_sec._height)
                {
                    if (!col_fir)
                        r.add_component<Collider>(Entity(i), {{j}});
                    else
                        col_fir->_collisions.push_back(j);
                    if (!col_sec)
                        r.add_component<Collider>(Entity(j), {{i}});
                    else
                        col_sec->_collisions.push_back(i);
                }
                else
                {
                    if (col_fir && !col_fir->_collisions.empty() && std::find(col_fir->_collisions.begin(), col_fir->_collisions.end(), j) != col_fir->_collisions.end())
                        col_fir->_collisions.erase(std::find(col_fir->_collisions.begin(), col_fir->_collisions.end(), j));
                    if (col_sec && !col_sec->_collisions.empty() && std::find(col_sec->_collisions.begin(), col_sec->_collisions.end(), i) != col_sec->_collisions.end())
                        col_sec->_collisions.erase(std::find(col_sec->_collisions.begin(), col_sec->_collisions.end(), i));
                }
            }
        }
    }
};

#endif /* !COLLIDINGSYSTEM_HPP_ */