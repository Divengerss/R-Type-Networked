#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

#define Max_End_Menu 2
class EndMenu
{
    public:
        EndMenu(int width, int height);

        void draw(sf::RenderWindow &window);
        void MoveUp();
        void MoveDown();

        int endMenuPressed() {
            return endMenuselected;
        }
        ~EndMenu();

    private:
        int endMenuselected;
        sf::Font font;
        sf::Text End_Menu[Max_End_Menu];
};