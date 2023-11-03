#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <stdexcept>

namespace rtype
{
    class Assets
    {
    public:
        static Assets &getInstance()
        {
            static Assets instance;
            return instance;
        }

        sf::Texture &getTexture(const std::string &textureName)
        {
            auto it = _textures.find(textureName);
            if (it == _textures.end())
            {
                throw std::runtime_error("Texture not found: " + textureName);
            }
            else
            {
                return it->second;
            }
        }

    private:
        Assets()
        {
#ifdef WIN32
            addTexture("space", ".\\assets\\sprites\\Space.png");
            addTexture("player", ".\\assets\\sprites\\r-typesheet42.gif");
            addTexture("enemy", ".\\assets\\sprites\\r-typesheet5.gif");
            addTexture("bullet", ".\\assets\\sprites\\r-typesheet2.gif");
#else
            addTexture("space", "./assets/sprites/Space.png");
            addTexture("player", "./assets/sprites/r-typesheet42.gif");
            addTexture("enemy", "./assets/sprites/r-typesheet5.gif");
            addTexture("bullet", "./assets/sprites/r-typesheet2.gif");
#endif
        }
        Assets(const Assets &) = delete;
        Assets &operator=(const Assets &) = delete;

        void addTexture(std::string textureName, std::string texturePath)
        {
            sf::Texture texture;
            texture.loadFromFile(texturePath);
            _textures[textureName] = texture;
        }

        std::unordered_map<std::string, sf::Texture> _textures;
    };
}