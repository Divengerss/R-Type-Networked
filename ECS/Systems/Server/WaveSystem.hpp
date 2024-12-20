/*
** EPITECH PROJECT, 2023
** Sans titre(Espace de travail)
** File description:
** WaveSystem
*/

#ifndef WAVESYSTEM_HPP_
#define WAVESYSTEM_HPP_

#include <cstdlib>
#include <iostream>

#include "Registry.hpp"
#include "Tag.hpp"
#include "Texture.hpp"
#include "Position.hpp"
#include "Scale.hpp"
#include "Velocity.hpp"
#include "MovementPattern.hpp"
#include "Destroyable.hpp"
#include "Hitbox.hpp"
#include "Damaging.hpp"

class WaveSystem
{
public:
    WaveSystem() = default;
    ~WaveSystem() = default;

    void run(Registry &r)
    {
        _enemy_count = 0;

        for (int i = 0; i < r.get_entity_number(); i++)
        {
            if (!r.entity_has_component<Tag>(Entity(i)))
                continue;
            auto const &tag = r.get_component<Tag>(Entity(i));
            if (tag._tag == TagEnum::ENEMY)
                _enemy_count++;
        }
        if (_enemy_count == 0)
        {
            _wave++;
            for (int i = 0; i < _wave * 2; i++)
            {
                int random = 0;
                int random_y = rand() % 1080;

                if (random == 1)
                    random = 0;

                Entity enemy = r.spawn_entity();

                r.add_component<Tag>(enemy, Tag(TagEnum::ENEMY));
                r.add_component<Position>(enemy, {1920, static_cast<float>(random_y)});
                r.add_component<Velocity>(enemy, {2});
                r.add_component<MovementPattern>(enemy, {static_cast<MovementPatterns>(random)});
                r.add_component<Destroyable>(enemy, {2});
                r.add_component<Hitbox>(enemy, {33, 36});
                r.add_component<Damaging>(enemy, {true});
            }
        }
    }

protected:
private:
    int _wave = -1;
    int _enemy_count = 0;
};

#endif /* !WaveSystem_HPP_ */
