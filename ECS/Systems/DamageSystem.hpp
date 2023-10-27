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
#include "Score.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

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
        for (int i = 0; i < r.get_entity_number(); i++) {
            if (!r.entity_has_component<Position>(Entity(i)) || !r.entity_has_component<Hitbox>(Entity(i)) || !r.entity_has_component<Destroyable>(Entity(i)))
                continue;
            auto &destroyable_dest = r.get_component<Destroyable>(Entity(i));

            auto const &sprite_dest = sprites[i].first;

             for (int j = 0; j < r.get_entity_number(); j++) {
                if (i == j)
                    continue;
                if (!r.entity_has_component<Position>(Entity(j)) || !r.entity_has_component<Hitbox>(Entity(j)) || !r.entity_has_component<Destroyable>(Entity(j)) || !r.entity_has_component<Damaging>(Entity(j)))
                    continue;
                auto const &sprite_from = sprites[j].first;

                if (sprite_from.getGlobalBounds().intersects(sprite_dest.getGlobalBounds())) {
                    std::cout << "EXPLOSION" << std::endl;
                    sound.play();
                    r.kill_entity(Entity(i));
                    destroyable_dest._hp = 0;
                    sprites.erase(i);
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
