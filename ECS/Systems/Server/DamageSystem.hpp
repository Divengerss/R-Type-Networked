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
        auto const positions = r.get_components<Position>();
        auto destroyable = r.get_components<Destroyable>();
        auto const hitboxes = r.get_components<Hitbox>();
        auto const damages = r.get_components<Damaging>();

        for (size_t i = 0; i < positions.size(); ++i)
        {
            auto const &hb_dest = hitboxes[i];
            auto const &pos_dest = positions[i];
            auto &dest = destroyable[i];
            if (pos_dest && hb_dest && dest)
            {
                for (size_t j = 0; j < positions.size(); ++j)
                {
                    if (i == j)
                        continue;
                    auto const &pos_dam = positions[j];
                    auto const &dam = damages[j];
                    // if (pos_dam.has_value())
                    //     std::cout << pos_dam.value()._x << " " << pos_dam.value()._y << std::endl;
                    if (pos_dest.has_value() && pos_dam.has_value() && dam) {
                        std::cout << pos_dest.value()._x << " " << pos_dest.value()._y << " // " << pos_dam.value()._x << " " << pos_dam.value()._y << std::endl;
                        std::cout << hb_dest.value()._width << " " << hb_dest.value()._height << std::endl;
                        // hitbox 1
                        float damLeft = pos_dam.value()._x;
                        float damTop = pos_dam.value()._y;
                        float damRight = damLeft + hb_dest.value()._width;
                        float damBottom = damTop + hb_dest.value()._height;

                        // hitbox 2
                        float destLeft = pos_dest.value()._x;
                        float destTop = pos_dest.value()._y;
                        float destRight = destLeft + hb_dest.value()._width;
                        float destBottom = destTop + hb_dest.value()._height;

                        if (damRight >= destLeft &&    // hitbox1 right edge past hitbox2 left
                            damLeft <= destRight &&    // hitbox1 left edge past hitbox2 right
                            damBottom >= destTop &&    // hitbox1 top edge past hitbox2 bottom
                            damTop <= destBottom) {    // hitbox1 bottom edge past hitbox2 top
                                r.kill_entity(Entity(i));
                        }
                    }
                    // if (/* intersection && */ dam)
                    // {
                    //     std::cout << "EXPLOSION" << std::endl;
                    //     r.kill_entity(Entity(i));
                    // }
                }
            }
        }
    }
    protected:
    private:
    };

#endif /* !DAMAGESYSTEM_HPP_ */
