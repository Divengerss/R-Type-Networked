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
#include "Sprite.hpp"
#include "Tag.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

class DamageSystem
{
public:
    DamageSystem()
    {
        sound.setBuffer(buffer);
        buffer.loadFromFile("assets/All SFX/SND.DAT_00027.wav");
    }
    ~DamageSystem() = default;

    void damageSystem(Registry &r)
    {
        for (int i = 0; i < r.get_entity_number(); i++)
        {
            if (!r.entity_has_component<Tag>(Entity(i)) ||
                !r.entity_has_component<Destroyable>(Entity(i)) ||
                !r.entity_has_component<Sprite>(Entity(i)))
                continue;

            auto &a_hp = r.get_component<Destroyable>(Entity(i));
            auto &a_sprite = r.get_component<Sprite>(Entity(i))._sprite;
            auto &a_tag = r.get_component<Tag>(Entity(i))._tag;

            for (int j = 0; j < r.get_entity_number(); j++)
            {
                if (i == j)
                    continue;

                if (!r.entity_has_component<Tag>(Entity(i)) ||
                    !r.entity_has_component<Destroyable>(Entity(i)) ||
                    !r.entity_has_component<Sprite>(Entity(i)))
                    continue;

                auto &b_hp = r.get_component<Destroyable>(Entity(j));
                auto &b_sprite = r.get_component<Sprite>(Entity(j))._sprite;
                auto &b_tag = r.get_component<Tag>(Entity(j))._tag;

                if (b_sprite.getGlobalBounds().intersects(a_sprite.getGlobalBounds()))
                {
                    sound.play();
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
    }
    sf::SoundBuffer buffer;
    sf::Sound sound;

protected:
private:
};

#endif /* !DAMAGESYSTEM_HPP_ */
