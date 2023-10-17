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
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

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
                    if (/* intersection && */ dam)
                    {
                        std::cout << "EXPLOSION" << std::endl;
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
