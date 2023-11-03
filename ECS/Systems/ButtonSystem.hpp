/*
** EPITECH PROJECT, 2023
** Sans titre(Espace de travail)
** File description:
** ButtonSystem
*/

#ifndef BUTTONSYSTEM_HPP_
#define BUTTONSYSTEM_HPP_

#include <cstdlib>
#include <iostream>

#include "Registry.hpp"
#include "Button.hpp"

class ButtonSystem
{
public:
    ButtonSystem() = default;
    ~ButtonSystem() = default;

    void run(Registry &r, sf::RenderWindow &window, bool mouse_clicked)
    {
        for (int i = 0; i < r.get_entity_number(); i++) {
            if (!r.entity_has_component<Button>(Entity(i)))
                continue;
            auto &button = r.get_component<Button>(Entity(i));

            if (button.isMouseOver(window)) {
                button.onHover();
            } else {
                button.onIdle();
            }

            if (mouse_clicked && button.isMouseOver(window)) {
                button.onClicked();
            }
        }
    }

protected:
private:
};

#endif /* !BUTTONSYSTEM_HPP_ */
