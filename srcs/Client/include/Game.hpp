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
    void updateSprite(Registry &t)
    {
        p.positionSystem(t);
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
            auto &pos = positions[i];
            auto scale = scales[i];
            auto &destroyable = destroyables[i];
            auto &hitbox = hitboxes[i];
            auto &damaging = damagings[i];
            if (texture && pos && _sprites.find(i) == _sprites.end())
            {
                std::cout << "sprite crée" << std::endl;
                sf::Sprite sprite;
                sf::Texture spriteTexture(texture->_texture);
                sprite.setTexture(spriteTexture);
                sprite.setPosition(pos->_x, pos->_y);
                sprite.setTextureRect(sf::IntRect(texture->_left, texture->_top, texture->_width, texture->_height));
                sprite.setScale(scale->_scaleX, scale->_scaleY);
                _sprites.emplace(i, std::pair<sf::Sprite, sf::Texture>(sprite, spriteTexture));
                _sprites[i].first.setTexture(_sprites[i].second);
            } else if (texture && pos) {
                sf::Sprite &sprite = _sprites.at(i).first;
                sprite.setPosition(pos->_x, pos->_y);
            }
        }
        d.damageSystem(t);
    };

    void drawSprite(sf::RenderWindow &window)
    {
        for (auto &sprite : _sprites)
            window.draw(sprite.second.first);
    }

protected:
private:
    std::map<size_t, std::pair<sf::Sprite, sf::Texture>> _sprites;
    sf::Clock Clock;
    PositionSystem p;
    DamageSystem d;
    float updateInterval = 1.0f;
};

#endif /* !GAME_HPP_ */