#include <SFML/Graphics.hpp>
#include "Screen.hpp"
#include "MainMenu.hpp"
using namespace sf;

Screen::Screen()
{
    RenderWindow window(VideoMode(800, 600), "Main Menu");
    mainMenu mainMenu(window.getSize().x, window.getSize().y);

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::KeyReleased)
            {
                if (event.key.code == Keyboard::Up)
                {
                    mainMenu.MoveUp();
                    break;
                }
                if (event.key.code == Keyboard::Down)
                {
                    mainMenu.MoveDown();
                    break;
                }
                if (event.key.code == Keyboard::Return)
                {
                    RenderWindow Play(VideoMode(800, 600), "R-type");
                    RenderWindow Options(VideoMode(800, 600), "Options");
                    RenderWindow Credits(VideoMode(800, 600), "Credits");
                    int x = mainMenu.mainMenuPressed();
                    if (x == 0)
                    {
                        while (Play.isOpen())
                        {
                            Event aevent;
                            while (Play.pollEvent(aevent))
                            {
                                if (aevent.type == Event::Closed)
                                    Play.close();
                                if (aevent.type == Event::KeyPressed)
                                {
                                    if (aevent.key.code == Keyboard::Escape)
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
                            Event aevent;
                            while (Options.pollEvent(aevent))
                            {
                                if (aevent.type == Event::Closed)
                                    Options.close();
                                if (aevent.type == Event::KeyPressed)
                                {
                                    if (aevent.key.code == Keyboard::Escape)
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
                            Event aevent;
                            while (Credits.pollEvent(aevent))
                            {
                                if (aevent.type == Event::Closed)
                                    Credits.close();
                                if (aevent.type == Event::KeyPressed)
                                {
                                    if (aevent.key.code == Keyboard::Escape)
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
                }
            }
        }
        window.clear();
        mainMenu.draw(window);
        window.display();
    }
}
