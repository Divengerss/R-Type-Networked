/*
** EPITECH PROJECT, 2023
** Sans titre(Espace de travail)
** File description:
** TextureSystem
*/

#ifndef TEXTURESYSTEM_HPP_
#define TEXTURESYSTEM_HPP_

#include "ISystem.hpp"

#include "Position.hpp"
#include "Controllable.hpp"
#include "Texture.hpp"
#include "MovementPattern.hpp"
#include "Scale.hpp"
#include "Velocity.hpp"
#include "Destroyable.hpp"
#include "Hitbox.hpp"
#include "Damaging.hpp"

#include <SFML/Graphics.hpp>

#include "Registry.hpp"
#include "Entity.hpp"

class TextureSystem : public ISystem
{
public:
    void updateSprites(Registry &r) {
        auto positions = r.get_components<Position>();
        auto velocities = r.get_components<Velocity>();
        auto textures = r.get_components<Texture>();
        auto scales = r.get_components<Scale>();
        for (std::size_t i = 0; i < textures.size(); ++i)
        {
            auto &texture = textures[i];
            auto &pos = positions[i];
            auto scale = scales[i];
            if (texture && pos && _sprites.find(i) == _sprites.end())
            {
                sf::Sprite sprite;
                sf::Texture spriteTexture = sf::Texture();
                spriteTexture.loadFromFile(texture->_path);
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
    };
    void drawSprites() {
        //for (auto &sprite : _sprites)
            //window.draw(sprite.second.first);
    };
    void damageSystem(Registry &r)
    {
        auto const positions = r.get_components<Position>();
        auto destroyable = r.get_components<Destroyable>();
        auto const hitboxes = r.get_components<Hitbox>();
        auto const damages = r.get_components<Damaging>();

        for (size_t i = 0; i < positions.size(); ++i)
        {
            auto const &hb_dest = hitboxes[i];
            auto const &pos_dest = positions[i];
            auto &dest = destroyable[i];
            auto &sprite_dest = _sprites[i].first;
            if (pos_dest && hb_dest && dest)
            {
                for (size_t j = 0; j < positions.size(); ++j)
                {
                    if (i == j)
                        continue;
                    auto const &sprite_dam = _sprites[j].first;
                    auto const &dam = damages[j];
                    if (sprite_dam.getGlobalBounds().intersects(sprite_dest.getGlobalBounds()) && dam)
                    {
                        r.kill_entity(Entity(i));
                        _sprites.erase(i);
                    }
                }
            }
        }
    }
    void textureSystem(Registry &r)
    {
        auto positions = r.get_components<Position>();
        auto controllable = r.get_components<Controllable>();
        auto velocities = r.get_components<Velocity>();
        auto textures = r.get_components<Texture>();
        auto movements = r.get_components<MovementPattern>();

        for (std::size_t i = 0; i < positions.size(); i++)
        {
            auto &texture = textures[i];
            auto &pos = positions[i];
            auto &cont = controllable[i];
            auto &move = movements[i];
            if (pos && cont && !texture.has_value())
            {
                #ifdef WIN32
                    r.add_component<Texture>(Entity(i), {"Release\\assets\\sprites\\r-typesheet42.gif", 66, createdPlayers * 18, 33, 17});
                #else
                    r.add_component<Texture>(Entity(i), {"./Release/assets/sprites/r-typesheet42.gif", 66, createdPlayers * 18, 33, 17});
                #endif
                r.add_component<Scale>(Entity(i), {3, 3});
                createdPlayers++;
            }
            else if (pos && !cont.has_value() && !texture.has_value())
            {
                if (move->_movementPattern == MovementPatterns::STRAIGHTLEFT)
                    #ifdef WIN32
                        r.add_component<Texture>(Entity(i), {"Release\\assets\\sprites\\r-typesheet5.gif", 233, 0, 33, 36});
                    #else
                        r.add_component<Texture>(Entity(i), {"./Release/assets/sprites/r-typesheet5.gif", 233, 0, 33, 36});
                    #endif
                if (move->_movementPattern == MovementPatterns::STRAIGHTRIGHT)
                    #ifdef WIN32
                        r.add_component<Texture>(Entity(i), {"Release\\assets\\sprites\\r-typesheet2.gif", 185, 0, 25, 25});
                    #else
                        r.add_component<Texture>(Entity(i), {"./Release/assets/sprites/r-typesheet2.gif", 185, 0, 25, 25});
                    #endif

                r.add_component<Scale>(Entity(i), {3, 3});
            }
        }
    }
    void runSystem(Registry &r) {
        textureSystem(r);
        updateSprites(r);
        damageSystem(r);
    }

    std::map<size_t, std::pair<sf::Sprite, sf::Texture>> _sprites;
    int createdPlayers = 0;

protected:
private:
};

#endif /* !TEXTURESYSTEM_HPP_ */
