#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

#include "EndMenu.hpp"

static constexpr int Max_Main_Menu = 4;

class mainMenu
{
    public:
        mainMenu(int width, int height);

        void draw(sf::RenderWindow &window);
        void MoveUp();
        void MoveDown();
        void setMenu(int index);

        int mainMenuPressed() {
            return mainMenuselected;
        }
        const menu::EndMenu &getEndMenu() {
            return endMenu;
        }
        ~mainMenu();

    private:
        int mainMenuselected;
        sf::Font font;
        sf::Text Main_Menu[Max_Main_Menu];
        menu::EndMenu endMenu;
};
