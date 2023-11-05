#pragma once

#include "Registry.hpp"
#include "HealthBar.hpp"
#include "Destroyable.hpp"
#include "Position.hpp"
#include "Tag.hpp"

#include <SFML/Graphics.hpp>

class HealthBarSystem
{
public:
    HealthBarSystem() = default;
    ~HealthBarSystem() = default;

    void run(Registry& reg)
    {
        for (int i = 0; i < reg.get_entity_number(); ++i) {
            if (reg.entity_has_component<HealthBar>(Entity(i))) {
                auto &healthBar = reg.get_component<HealthBar>(Entity(i));
                auto &destroyable = reg.get_component<Destroyable>(Entity(i));
                auto &pos = reg.get_component<Position>(Entity(i));

                healthBar.update(destroyable._hp, pos._x, pos._y - 20);

                continue;
            }

            if (reg.entity_has_component<HealthBar>(Entity(i))) continue;
            if (reg.entity_has_component<Destroyable>(Entity(i)) == false || reg.entity_has_component<Tag>(Entity(i)) == false) continue;


            auto &destroyable = reg.get_component<Destroyable>(Entity(i));
            auto &tag = reg.get_component<Tag>(Entity(i));

            if (tag._tag == TagEnum::PLAYER) {
                reg.add_component<HealthBar>(Entity(i), {90, 20, destroyable._hp, sf::Color::Green});
            } else if (tag._tag == TagEnum::ENEMY) {
                reg.add_component<HealthBar>(Entity(i), {90, 20, destroyable._hp, sf::Color::Red});
            }
        }
    }
};
