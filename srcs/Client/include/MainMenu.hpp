#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

class mainMenu
{
    public:
        mainMenu(int width, int height);

        void draw(sf::RenderWindow &window);
        void MoveUp();
        void MoveDown();

        int mainMenuPressed() {
            return mainMenuselected;
        }
        ~mainMenu();

    private:
        int mainMenuselected;
        sf::Font font;
        sf::Text Main_Menu[4];
};
