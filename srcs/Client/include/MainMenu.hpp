#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

#define Max_Main_Menu 4
class mainMenu
{
    public:
    mainMenu(float width, float height);

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
    sf::Text Main_Menu[Max_Main_Menu];
};
