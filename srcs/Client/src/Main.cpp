#include "Client.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

int main(int argc, const char *argv[]) {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");

    sf::Texture textureSpace;

    if (!textureSpace.loadFromFile("./Release/assets/Backgrounds/Space.png"))
        return EXIT_FAILURE;
    sf::Sprite Space(textureSpace);
    sf::IntRect SpaceToDisplay(0, 0, 1587, 200);
    Space.setTextureRect(SpaceToDisplay);
    Space.setScale(3, 3);

    sf::Texture texture;
    if (!texture.loadFromFile("./Release/assets/sprites/ship.gif"))
        return EXIT_FAILURE;
    sf::Sprite Ship(texture);
    sf::IntRect ShipToDisplay(75, 50, 25, 20);
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
        window.draw(Space);
        window.draw(Ship);
        window.display();
    }
    return EXIT_SUCCESS;
    return 0;
}
