#include <SFML/Graphics.hpp>
#include "Screen.hpp"
#include "MainMenu.hpp"
#include "EndMenu.hpp"

Screen::Screen()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Main Menu");
    mainMenu mainMenu(window.getSize().x, window.getSize().y);
    EndMenu endMenu(window.getSize().x, window.getSize().y);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyReleased)
            {
                if (event.key.code == sf::Keyboard::Up)
                {
                    mainMenu.MoveUp();
                    break;
                }
                if (event.key.code == sf::Keyboard::Down)
                {
                    mainMenu.MoveDown();
                    break;
                }
                if (event.key.code == sf::Keyboard::Return)
                {
                    sf::RenderWindow Play(sf::VideoMode(800, 600), "R-type");
                    sf::RenderWindow Options(sf::VideoMode(800, 600), "Options");
                    sf::RenderWindow Credits(sf::VideoMode(800, 600), "Credits");
                    sf::RenderWindow End(sf::VideoMode(800, 600), "End Menu");
                    int y = endMenu.endMenuPressed();
                    int x = mainMenu.mainMenuPressed();
                    if (x == 0)
                    {
                        while (Play.isOpen())
                        {
                            sf::Event aevent;
                            while (Play.pollEvent(aevent))
                            {
                                if (aevent.type == sf::Event::Closed)
                                    Play.close();
                                if (aevent.type == sf::Event::KeyPressed)
                                {
                                    if (aevent.key.code == sf::Keyboard::Escape)
                                        Play.close();
                                }
                            }
                            Options.close();
                            Credits.close();
                            Play.clear();
                            Play.display();
                        }
                    }
                    if (x == 1)
                    {
                        while (Options.isOpen())
                        {
                            sf::Event aevent;
                            while (Options.pollEvent(aevent))
                            {
                                if (aevent.type == sf::Event::Closed)
                                    Options.close();
                                if (aevent.type == sf::Event::KeyPressed)
                                {
                                    if (aevent.key.code == sf::Keyboard::Escape)
                                        Options.close();
                                }
                            }
                            Play.close();
                            Credits.close();
                            Options.clear();
                            Options.display();
                        }
                    }
                    if (x == 2)
                    {
                        while (Credits.isOpen())
                        {
                            sf::Event aevent;
                            while (Credits.pollEvent(aevent))
                            {
                                if (aevent.type == sf::Event::Closed)
                                    Credits.close();
                                if (aevent.type == sf::Event::KeyPressed)
                                {
                                    if (aevent.key.code == sf::Keyboard::Escape)
                                        Credits.close();
                                }
                            }
                            Play.close();
                            Options.close();
                            Credits.clear();
                            Credits.display();
                        }
                    }
                    if (x == 3)
                        window.close();
                    break;
                    if (y == 0)
                    {
                        while (Play.isOpen())
                        {
                            sf::Event aevent;
                            while (Play.pollEvent(aevent))
                            {
                                if (aevent.type == sf::Event::Closed)
                                    Play.close();
                                if (aevent.type == sf::Event::KeyPressed)
                                {
                                    if (aevent.key.code == sf::Keyboard::Escape)
                                        Play.close();
                                }
                            }
                            Options.close();
                            Credits.close();
                            Play.clear();
                            Play.display();
                        }
                    }
                    if (y == 1) {
                        while (Options.isOpen())
                        {
                            sf::Event aevent;
                            while (Options.pollEvent(aevent))
                            {
                                if (aevent.type == sf::Event::Closed)
                                    Options.close();
                                if (aevent.type == sf::Event::KeyPressed)
                                {
                                    if (aevent.key.code == sf::Keyboard::Escape)
                                        Options.close();
                                }
                            }
                            Play.close();
                            Credits.close();
                            Options.clear();
                            Options.display();
                        }
                    }
                    if (y == 2)
                        window.close();
                }
            }
        }
        window.clear();
        mainMenu.draw(window);
        window.display();
    }
}
