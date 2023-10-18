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

    void damageSystem(Registry &r, std::map<size_t, std::pair<sf::Sprite, sf::Texture>> &sprites)
    {
        auto const positions = r.get_components<Position>();
        auto destroyable = r.get_components<Destroyable>();
        auto const hitboxes = r.get_components<Hitbox>();
        auto const damages = r.get_components<Damaging>();

        // std::cout << "positions.size(): " << positions.size() << std::endl;
        for (size_t i = 0; i < positions.size(); ++i)
        {
            // std::cout << "positions.size(): " << positions.size() << std::endl;
            // std::cout << "i: " << i << std::endl;
            auto const &hb_dest = hitboxes[i];
            auto const &pos_dest = positions[i];
            auto &dest = destroyable[i];
            auto &sprite_dest = sprites[i].first;
            if (pos_dest && hb_dest && dest)
            {
                for (size_t j = 0; j < positions.size(); ++j)
                {
                    if (i == j)
                        continue;
                    auto const &sprite_dam = sprites[j].first;
                    auto const &dam = damages[j];
                    if (sprite_dam.getGlobalBounds().intersects(sprite_dest.getGlobalBounds()) && dam)
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
