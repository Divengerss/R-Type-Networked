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
#include "ISystem.hpp"

class DamageSystem : public ISystem
{
public:
    DamageSystem() = default;
    ~DamageSystem() = default;

    void runSystem(Registry &r)
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
                    auto const &hb_dam = hitboxes[j];
                    if (pos_dam && dam && hb_dam) {
                        // hitbox 1
                        float damLeft = pos_dam.value()._x;
                        float damTop = pos_dam.value()._y;
                        float damRight = damLeft + hb_dam.value()._width;
                        float damBottom = damTop + hb_dam.value()._height;

                        // hitbox 2
                        float destLeft = pos_dest.value()._x;
                        float destTop = pos_dest.value()._y;
                        float destRight = destLeft + hb_dest.value()._width;
                        float destBottom = destTop + hb_dest.value()._height;

                        if (damLeft == damRight || damTop == damBottom || destRight == destLeft || destTop == destBottom)
                            continue;
                        if (damLeft > destRight || destLeft > damRight)
                            continue;
                        if (damBottom > destTop || destBottom > damTop)
                            continue;
                        r.kill_entity(Entity(i));
                    }
                }
            }
        }
    }
    protected:
    private:
    };

#endif /* !DAMAGESYSTEM_HPP_ */
