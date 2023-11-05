#pragma once

#include "Registry.hpp"
#include "Assets.hpp"
#include "Texture.hpp"
#include "Position.hpp"
#include "Scale.hpp"
#include "Sprite.hpp"

class SpriteSystem
{
public:
    SpriteSystem() = default;
    ~SpriteSystem() = default;

    void run(Registry& reg, rtype::Assets& _assets)
    {
        for (int i = 0; i < reg.get_entity_number(); i++) {
            if (!reg.entity_has_component<Texture>(Entity(i)))
                continue;

            auto &texture = reg.get_component<Texture>(Entity(i));
            auto &pos = reg.get_component<Position>(Entity(i));
            auto &scale = reg.get_component<Scale>(Entity(i));

            if (!reg.entity_has_component<Sprite>(Entity(i))) {
                sf::Texture &spriteTexture = _assets.getTexture(reg.get_component<Texture>(Entity(i))._path);
                reg.add_component<Sprite>(Entity(i), {spriteTexture});
                auto &sprite = reg.get_component<Sprite>(Entity(i))._sprite;

                sprite.setPosition(pos._x, pos._y);
                sprite.setTextureRect(sf::IntRect(texture._left, texture._top, texture._width, texture._height));
                sprite.setScale(scale._scaleX, scale._scaleY);
            } else {
                auto &sprite = reg.get_component<Sprite>(Entity(i))._sprite;
                sprite.setPosition(pos._x, pos._y);
            }
        }
    }
};
