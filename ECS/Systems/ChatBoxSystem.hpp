#pragma once

#include <SFML/Graphics.hpp>

#include "Registry.hpp"
#include "Network.hpp"

#include "ChatBox.hpp"

class ChatBoxSystem
{
public:
    ChatBoxSystem() = default;
    ~ChatBoxSystem() = default;

    void run(sf::Event event, Registry &reg, net::Client &client)
    {
        if (event.type == sf::Event::KeyReleased)
        {
            if (event.key.code == sf::Keyboard::Escape)
            {
                for (int i = 0; i < reg.get_entity_number(); ++i)
                {
                    if (reg.entity_has_component<ChatBox>(Entity(i)) == false) continue;

                    auto &chatbox = _reg.get_component<ChatBox>(Entity(i));
                    chatbox._isInputSelected = !chatbox._isInputSelected;
                }
            }
        }
        if (event.type == sf::Event::TextEntered)
        {
            for (int i = 0; i < reg.get_entity_number(); ++i)
            {
                if (reg.entity_has_component<ChatBox>(Entity(i)) == false) continue;

                auto &chatbox = reg.get_component<ChatBox>(Entity(i));

                if (chatbox._isInputSelected == false) continue;

                if (event.text.unicode < 128 && event.text.unicode != 36)
                {
                    if (event.text.unicode == 13)
                    {
                        packet::packetHeader header(packet::TEXT_MESSAGE, sizeof(packet::textMessage));
                        packet::textMessage textMessage(client.getUuid(), chatbox._inputString);
                        client.sendPacket(header, textMessage);
                        chatbox._inputString = "";
                        chatbox._inputText.setString("");
                    } else if (event.text.unicode == 8)
                    {
                        chatbox.removeChar();
                    } else {
                        chatbox.addChar(event.text.unicode);
                    }
                }
            }
        }
    }
};