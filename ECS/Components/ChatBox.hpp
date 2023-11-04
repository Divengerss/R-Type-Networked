/*
** EPITECH PROJECT, 2023
** r-type
** File description:
** Button
*/

#ifndef CHATBOX_HPP_
#define CHATBOX_HPP_

#include <SFML/Graphics.hpp>
#include <string>

class ChatBox {
    public:
        ChatBox(float posX, float posY, float width, float height)
        {
            _shape.setPosition(posX, posY);
            _shape.setSize(sf::Vector2f(width, height));
            _shape.setFillColor(sf::Color::Transparent);

            _input.setPosition(posX + 10, posY + height - 40);
            _input.setSize(sf::Vector2f(width - 20, 30));
            _input.setFillColor(sf::Color::White);

            _inputText.setPosition(posX + 10, posY + height - 40);
            _inputText.setFillColor(sf::Color::Black);
            _inputText.setString("Enter your message here");
        };

        void draw(sf::RenderWindow &window, sf::Font &font)
        {
            if (_isInputSelected == false) return;

            window.draw(_shape);
            for (auto &message : _messages) {
                message.setFont(font);
                window.draw(message);
            }
            window.draw(_input);
            _inputText.setFont(font);
            window.draw(_inputText);
        }

        void addMessage(const std::string &message)
        {
            sf::Text text;
            text.setString(message);
            text.setFillColor(sf::Color::White);
            text.setPosition(_shape.getPosition().x + 10, _shape.getPosition().y + 10 + _messages.size() * 30);
            if (_messages.size() == 5) {
                _messages.erase(_messages.begin());
                for (int i = 0; i < _messages.size(); i++) {
                    _messages[i].setPosition(_shape.getPosition().x + 10, _shape.getPosition().y + 10 + i * 30);
                }
            }
            _messages.push_back(text);
        }

        void addChar(char c)
        {
            _inputString += c;
            _inputText.setString(_inputString);
        }

        void removeChar()
        {
            if (_inputString.size() == 0) return;
            _inputString = "";
            _inputText.setString(_inputString);
        }

        bool _isInputSelected = false;

        sf::RectangleShape _shape;
        std::vector<sf::Text> _messages;

        sf::RectangleShape _input;
        std::string _inputString;
        sf::Text _inputText;
};

#endif /* !CHATBOX_HPP_ */
