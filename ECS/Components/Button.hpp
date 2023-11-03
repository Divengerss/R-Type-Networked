/*
** EPITECH PROJECT, 2023
** r-type
** File description:
** Button
*/

#ifndef BUTTON_HPP_
#define BUTTON_HPP_

#include <SFML/Graphics.hpp>
#include <string>

class Button {
    public:
        Button(float posX, float posY, float width, float height, const std::string &text, const sf::Font &font, std::function<void()> callback)
        {
            _shape.setPosition(posX, posY);
            _shape.setSize(sf::Vector2f(width, height));
            _shape.setFillColor(sf::Color::Transparent);

            _text.setFont(font);
            _text.setString(text);
            _text.setFillColor(sf::Color::White);
            sf::FloatRect textRect = _text.getLocalBounds();
            _text.setOrigin(textRect.left + textRect.width / 2.0f,
                            textRect.top + textRect.height / 2.0f);
            _text.setPosition(sf::Vector2f(posX + width / 2.0f, posY + height / 2.0f));

            _callback = callback;
        };

        bool isMouseOver(sf::RenderWindow &window)
        {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

            if (_shape.getGlobalBounds().contains(mousePosF))
                return true;
            return false;
        }

        void onHover()
        {
            _shape.setFillColor(sf::Color::Red);
        }

        void onIdle()
        {
            _shape.setFillColor(sf::Color::Transparent);
        }

        void onClicked()
        {
            _callback();
        }

        void draw(sf::RenderWindow &window)
        {
            window.draw(_shape);
            window.draw(_text);
        }

        sf::RectangleShape _shape;
        sf::Text _text;
        std::function<void()> _callback;
};

#endif /* !BUTTON_HPP_ */
