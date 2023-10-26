#include "EndMenu.hpp"

EndMenu::EndMenu(int width, int height)
{
    if (!font.loadFromFile("assets/arial.ttf")) {
        std::cerr << "Error loading arial.ttf" << std::endl;
    }
    End_Menu[0].setFont(font);
    End_Menu[0].setFillColor(sf::Color::Red);
    End_Menu[0].setString("Main Menu");
    End_Menu[0].setPosition(sf::Vector2f(width / 2, height / (Max_End_Menu + 1) * 1));

    End_Menu[1].setFont(font);
    End_Menu[1].setFillColor(sf::Color::White);
    End_Menu[1].setString("Exit");
    End_Menu[1].setPosition(sf::Vector2f(width / 2, height / (Max_End_Menu + 1) * 2));

    endMenuselected = 0;
}

EndMenu::~EndMenu()
{
}

void EndMenu::draw(sf::RenderWindow &window)
{
    for (int i = 0; i < Max_End_Menu; i++) {
        window.draw(End_Menu[i]);
    }
}

void EndMenu::MoveUp()
{
    if (endMenuselected - 1 >= -1) {
        End_Menu[endMenuselected].setFillColor(sf::Color::White);
        endMenuselected--;
        if (endMenuselected == -1)
            endMenuselected = Max_End_Menu - 1;
        End_Menu[endMenuselected].setFillColor(sf::Color::Red);
    }
}

void EndMenu::MoveDown()
{
    if (endMenuselected + 1 <= Max_End_Menu) {
        End_Menu[endMenuselected].setFillColor(sf::Color::White);
        endMenuselected++;
        if (endMenuselected == Max_End_Menu)
            endMenuselected = 0;
        End_Menu[endMenuselected].setFillColor(sf::Color::Red);
    }
}