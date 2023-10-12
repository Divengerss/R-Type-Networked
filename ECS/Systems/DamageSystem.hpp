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
    DamageSystem();
    ~DamageSystem();

    void damageSystem(Registry &r)
    {
        auto const hitboxes = r.get_components<Hitbox>();
        auto const positions = r.get_components<Position>();
        auto const damages = r.get_components<Damaging>();
        auto destroyable = r.get_components<Destroyable>();

        for (size_t i = 0; i < positions.size(); ++i)
        {
            auto const &hb_dest = hitboxes[i];
            auto const &pos_dest = positions[i];
            auto &dest = destroyable[i];
            if (pos_dest && hb_dest && dest)
            {
                for (size_t j = 0; j < positions.size(); ++j)
                {
                    if (i = j)
                        continue;
                    auto const &hb_dam = hitboxes[j];
                    auto const &pos_dam = positions[j];
                    auto const &dam = damages[j];
                    if (pos_dam && hb_dam && dam)
                    {
                        if (pos_dest.value()._x < pos_dam.value()._x + hb_dam.value()._width &&
                            pos_dest.value()._x + hb_dest.value()._width > pos_dam.value()._x &&
                            pos_dest.value()._y < pos_dam.value()._y + hb_dam.value()._height &&
                            pos_dest.value()._y + hb_dest.value()._height > pos_dam.value()._y)
                            // COLLISION
                            dest.value()._hp -= dam.value()._damages;
                    }
                }
            }
        }
    }

protected:
private:
};

#endif /* !DAMAGESYSTEM_HPP_ */
