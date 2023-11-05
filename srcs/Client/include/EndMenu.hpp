#ifndef _END_MENU_HPP
#define _END_MENU_HPP
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

static constexpr int Max_End_Menu = 2;

namespace menu
{
    class EndMenu
    {
    public:
        EndMenu() = delete;
        EndMenu(int width, int height);

        void draw(sf::RenderWindow& window);
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
}

#endif /* !_END_MENU_HPP */