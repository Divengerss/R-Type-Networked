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

        for (std::size_t i = 0; i < textures.size(); ++i)
        {
            auto &texture = textures[i];
            auto pos = positions[i];
            auto scale = scales[i];
            PositionSystem p;
            std::cout << texture->_path << " " << texture->_left << " " << texture->_height << std::endl;
            if (texture && pos)
            {
                // MyRect myRect;
                // myRect.left = 0;
                // myRect.top = 0;
                // myRect.width = 950;
                // myRect.height = 200;
                //////////////////////
                sf::Sprite sprite(texture->_texture);
                sprite.setPosition(pos->_x, pos->_y);
                sprite.setTextureRect(sf::IntRect(texture->_left, texture->_top, texture->_width, texture->_height));
                sprite.setScale(scale->_scaleX, scale->_scaleY);
                p.positionSystem(t);
                window.draw(sprite);
            }
        }
    };
    void drawSprite(Registry &t);

protected:
private:
};

#endif /* !GAME_HPP_ */