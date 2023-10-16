/*
** EPITECH PROJECT, 2023
** r-type
** File description:
** Game
*/

#ifndef GAME_HPP_
#define GAME_HPP_

#include "Texture.hpp"
#include "Registry.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "Scale.hpp"
#include "PositionSystem.hpp"
#include "DamageSystem.hpp"

struct MyRect
{
    int left;
    int top;
    int width;
    int height;
};

class Game
{
public:
    // Game();
    // ~Game();
    void updateSprite(Registry &t, sf::RenderWindow &window)
    {
        auto positions = t.get_components<Position>();
        auto velocities = t.get_components<Velocity>();
        auto textures = t.get_components<Texture>();
        auto scales = t.get_components<Scale>();
        auto destroyables = t.get_components<Destroyable>();
        auto hitboxes = t.get_components<Hitbox>();
        auto damagings = t.get_components<Damaging>();

        for (std::size_t i = 0; i < textures.size(); ++i)
        {
            auto &texture = textures[i];
            auto pos = positions[i];
            auto scale = scales[i];
            auto &destroyable = destroyables[i];
            auto &hitbox = hitboxes[i];
            auto &damaging = damagings[i];

            if (texture && pos)
            {
                sf::Sprite sprite(texture->_texture);
                sprite.setPosition(pos->_x, pos->_y);
                sprite.setTextureRect(sf::IntRect(texture->_left, texture->_top, texture->_width, texture->_height));
                sprite.setScale(scale->_scaleX, scale->_scaleY);
                p.positionSystem(t);
                d.damageSystem(t);
                window.draw(sprite);
            }
        }
    };
    void drawSprite(Registry &t);

protected:
private:
    sf::Clock Clock;
    PositionSystem p;
    DamageSystem d;
    float updateInterval = 1.0f;
};

#endif /* !GAME_HPP_ */