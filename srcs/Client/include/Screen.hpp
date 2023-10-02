#pragma one
#include <SFML/Graphics.hpp>
#include <iostream>

class Screen {
    public:
        Screen();
        ~Screen() = default;
    private:
        sf::RenderWindow window;
        sf::Event event;
        sf::Texture texture;
        sf::Sprite sprite;
        sf::Font font;
};

