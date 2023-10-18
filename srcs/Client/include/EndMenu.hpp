#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

#define Max_End_Menu 2
class endMenu
{
    public:
        endMenu(int width, int height);

        void draw(sf::RenderWindow &window);
        void MoveUp();
        void MoveDown();

        int endMenuPressed() {
            return endMenuselected;
        }
        ~endMenu();

    private:
        int endMenuselected;
        sf::Font font;
        sf::Text End_Menu[Max_End_Menu];
};