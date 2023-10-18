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
#include <SFML/Audio.hpp>

class DamageSystem
{
public:
    DamageSystem() {
        sound.setBuffer(buffer);
        buffer.loadFromFile("assets/All SFX/SND.DAT_00027.wav");
    }
    ~DamageSystem() = default;

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
                        std::cout << "EXPLOSION" << std::endl;
                        sound.play();
                        r.kill_entity(Entity(i));
                        sprites.erase(i);
                    }
                }
            }
        }
    }
    sf::SoundBuffer buffer;
    sf::Sound sound;

protected:
private:
};

#endif /* !DAMAGESYSTEM_HPP_ */
