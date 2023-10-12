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

        for (int i = 0; i < textures.size(); ++i)
        {
            auto texture = textures[i];
            auto pos = positions[i];
            auto scale = scales[i];
            if (texture && pos)
            {
                MyRect myRect;
                myRect.left = 0;
                myRect.top = 0;
                myRect.width = 950;
                myRect.height = 200;
                // std::cout << texture->_path << std::endl;
                // std::cout << pos->_x << " " << pos->_y << std::endl;
                sf::Sprite sprite(texture->_texture);
                sprite.setPosition(pos->_x, pos->_y);
                sprite.setTextureRect(sf::IntRect(myRect.left, myRect.top, myRect.width, myRect.height));
                sprite.setScale(scale->_scaleX, scale->_scaleY);
                BackgroundScrolling(pos);
                window.draw(sprite);
            }
        }
    };
    void drawSprite(Registry &t);

    void BackgroundScrolling(auto pos)
    {
        std::cout << pos->_x << std::endl;
        pos->_x -= 1;
        std::cout << pos->_x << std::endl;
        // myRect.left += _spaceScrollSpeed;
        // if (_SpaceToDisplay.left >= 950)
        //     _SpaceToDisplay.left = 0;
        // _Space.setTextureRect(_SpaceToDisplay);
    };

protected:
private:
    // std::vecctor<
};

#endif /* !GAME_HPP_ */