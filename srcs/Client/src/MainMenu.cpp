#include "MainMenu.hpp"

mainMenu::mainMenu(int width, int height) : endMenu(width, height)
{
    if (!font.loadFromFile("assets/arial.ttf")) {
        std::cout << "Error loading file" << std::endl;
    }
    Main_Menu[0].setFont(font);
    Main_Menu[0].setFillColor(sf::Color::Red);
    Main_Menu[0].setString("Play");
    Main_Menu[0].setPosition(sf::Vector2f(static_cast < float > (width / 2), static_cast < float >(height / (Max_Main_Menu + 1) * 1)));

    Main_Menu[1].setFont(font);
    Main_Menu[1].setFillColor(sf::Color::White);
    Main_Menu[1].setString("Options");
    Main_Menu[1].setPosition(sf::Vector2f(static_cast < float > (width / 2), static_cast < float >(height / (Max_Main_Menu + 1) * 2)));

    Main_Menu[2].setFont(font);
    Main_Menu[2].setFillColor(sf::Color::White);
    Main_Menu[2].setString("Credits");
    Main_Menu[2].setPosition(sf::Vector2f(static_cast < float > (width / 2), static_cast < float >(height / (Max_Main_Menu + 1) * 3)));

    Main_Menu[3].setFont(font);
    Main_Menu[3].setFillColor(sf::Color::White);
    Main_Menu[3].setString("Exit");
    Main_Menu[3].setPosition(sf::Vector2f(static_cast < float > (width / 2), static_cast < float >(height / (Max_Main_Menu + 1) * 4)));

    mainMenuselected = 0;
}

mainMenu::~mainMenu()
{
}

void mainMenu::draw(sf::RenderWindow &window)
{
    for (int i = 0; i < Max_Main_Menu; i++) {
        window.draw(Main_Menu[i]);
    }
}

void mainMenu::MoveUp()
{
    if (mainMenuselected - 1 >= -1) {
        Main_Menu[mainMenuselected].setFillColor(sf::Color::White);
        mainMenuselected--;
        if (mainMenuselected == -1)
            mainMenuselected = Max_Main_Menu - 1;
        Main_Menu[mainMenuselected].setFillColor(sf::Color::Red);
    }
}

void mainMenu::MoveDown()
{
    if (mainMenuselected + 1 < Max_Main_Menu) {
        Main_Menu[mainMenuselected].setFillColor(sf::Color::White);
        mainMenuselected++;
        if (mainMenuselected == Max_Main_Menu)
            mainMenuselected = 0;
        Main_Menu[mainMenuselected].setFillColor(sf::Color::Red);
    }
}

void mainMenu::setMenu(int index)
{
    mainMenuselected = index;
}
