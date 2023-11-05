#pragma once

#include "Registry.hpp"
#include "ChatBox.hpp"
#include "HealthBar.hpp"
#include "Assets.hpp"

#include <SFML/Graphics.hpp>

class DrawSystem
{
public:
    DrawSystem() = default;
    ~DrawSystem() = default;

    void run(Registry& reg, sf::RenderWindow& window, rtype::Assets& _assets)
    {
        for (int i = 0; i < reg.get_entity_number(); ++i)
        {
            if (reg.entity_has_component<Sprite>(Entity(i)))
            {
                auto &sprite = reg.get_component<Sprite>(Entity(i));
                window.draw(sprite._sprite);
            }
            if (reg.entity_has_component<ChatBox>(Entity(i)))
            {
                auto &chatbox = reg.get_component<ChatBox>(Entity(i));
                chatbox.draw(window, _assets.getFont("arial"));
            }
            if (reg.entity_has_component<HealthBar>(Entity(i)))
            {
                auto &healthBar = reg.get_component<HealthBar>(Entity(i));
                healthBar.draw(window);
            }
        }
    }
};
