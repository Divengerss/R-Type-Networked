#include "Client.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

int main(int argc, const char *argv[]) {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");
    window.setFramerateLimit(60);
    sf::Texture textureSpace;

    if (!textureSpace.loadFromFile("./Release/assets/Backgrounds/Space.png"))
        return EXIT_FAILURE;
    sf::Sprite Space(textureSpace);
    sf::IntRect SpaceToDisplay(0, 0, 950, 200);
    Space.setTextureRect(SpaceToDisplay);
    Space.setScale(3, 3);
    float spaceScrollSpeed = 1.0f;

    sf::Texture texture;
    if (!texture.loadFromFile("./Release/assets/sprites/ship.gif"))
        return EXIT_FAILURE;
    sf::Sprite Ship(texture);
    sf::IntRect ShipToDisplay(66.4, 51.6, 33.2, 17.2);
    Ship.setTextureRect(ShipToDisplay);
    Ship.setScale(2, 2);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        SpaceToDisplay.left += spaceScrollSpeed;
        if (SpaceToDisplay.left >= 950)
            SpaceToDisplay.left = 0;
        Space.setTextureRect(SpaceToDisplay);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            Ship.setPosition(Ship.getPosition().x - 1, Ship.getPosition().y);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            Ship.setPosition(Ship.getPosition().x + 1, Ship.getPosition().y);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            Ship.setPosition(Ship.getPosition().x, Ship.getPosition().y - 1);
            ShipToDisplay.left += 33.2;
            if (ShipToDisplay.left >= 132.8)
                ShipToDisplay.left = 132.8;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            Ship.setPosition(Ship.getPosition().x, Ship.getPosition().y + 1);
            ShipToDisplay.left -= 33.2;
            if (ShipToDisplay.left <= 0)
                ShipToDisplay.left = 0;
        }
        Ship.setTextureRect(ShipToDisplay);
        ShipToDisplay.left = 66.4;
        window.clear();
        window.draw(Space);
        window.draw(Ship);
        window.display();
    }
    return EXIT_SUCCESS;
    return 0;
}
