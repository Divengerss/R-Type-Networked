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
#include "Collider.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

class DamageSystem
{
public:
    DamageSystem()
    {
        buffer.loadFromFile("assets/All SFX/SND.DAT_00027.wav");
        sound.setBuffer(buffer);
        sound.setLoop(false);
    }
    ~DamageSystem() = default;

    void damageSystem(Registry &r)
    {
        nbPlayerInvincible = 0;

        for (int i = 0; i < r.get_entity_number(); i++)
        {
            if (!r.entity_has_component<Tag>(Entity(i)) ||
                !r.entity_has_component<Destroyable>(Entity(i)) ||
                !r.entity_has_component<Collider>(Entity(i)))
                continue;

            auto &a_hp = r.get_component<Destroyable>(Entity(i));
            auto &a_tag = r.get_component<Tag>(Entity(i))._tag;
            auto &a_col = r.get_component<Collider>(Entity(i));

            if (!a_col._collisions.empty())
            {
                for (size_t k = 0; k < a_col._collisions.size(); k++)
                {
                    if (a_tag == TagEnum::PLAYER && a_hp._invincible) {
                        nbPlayerInvincible++;
                    }

                    if (!r.entity_has_component<Tag>(Entity(a_col._collisions[k])) ||
                        !r.entity_has_component<Damaging>(Entity(a_col._collisions[k])))
                        continue;

                    if (a_tag == TagEnum::BULLET && r.get_components<Tag>()[a_col._collisions[k]]->_tag == TagEnum::ENEMY && r.get_components<Destroyable>()[a_col._collisions[k]]->_hp > 0)
                    {
                        a_hp._hp = 0;
                        r.get_components<Destroyable>()[a_col._collisions[k]]->_hp = 0;
                        sound.play();
                    }
                }
            }
        }

        if (nbPlayerInvincible > lastNbPlayerInvincible) {
            sound.play();
        }
        lastNbPlayerInvincible = nbPlayerInvincible;
    }
    sf::SoundBuffer buffer;
    sf::Sound sound;
    int nbPlayerInvincible = 0;
    int lastNbPlayerInvincible = 0;

protected:
private:
};

#endif /* !DAMAGESYSTEM_HPP_ */
