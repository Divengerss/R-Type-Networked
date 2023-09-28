#include "Client.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include "Player.hpp"
#include "Background.hpp"

int main(int argc, const char *argv[]) {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");
    window.setFramerateLimit(60);

    // sf::Texture textureSpace;
    // if (!textureSpace.loadFromFile("./Release/assets/Backgrounds/Space.png"))
    //     return EXIT_FAILURE;
    // sf::Sprite Space(textureSpace);
    // sf::IntRect SpaceToDisplay(0, 0, 950, 200);
    // Space.setTextureRect(SpaceToDisplay);
    // Space.setScale(3, 3);
    // float spaceScrollSpeed = 1.0f;

    Background Space;
    Player Ship;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // SpaceToDisplay.left += spaceScrollSpeed;
        // if (SpaceToDisplay.left >= 950)
        //     SpaceToDisplay.left = 0;
        // Space.setTextureRect(SpaceToDisplay);
        Space.BackgroundScrolling();
        Ship.Movement();
        Ship.getSprite().setTextureRect(Ship.getShipToDisplay());
        window.clear();
        window.draw(Space.getSprite());
        window.draw(Ship.getSprite());
        window.display();
    }
    return EXIT_SUCCESS;
    return 0;
}