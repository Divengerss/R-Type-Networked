/*
** EPITECH PROJECT, 2023
** Sans titre(Espace de travail)
** File description:
** Controllable
*/

#ifndef CONTROLLABLE_HPP_
#define CONTROLLABLE_HPP_

#include <string>
#include <cstring>

#include "Uuid.hpp"

class Controllable {
    public:
        Controllable(const std::string &playerId) {
            std::memmove(_playerId.data(), playerId.c_str(), uuidSize);
        };
        Controllable(std::array<std::uint8_t, uuidSize> &playerId) {
            _playerId = playerId;
        };

        std::array<std::uint8_t, uuidSize> _playerId;
        int latestInput = -1;
};

#endif /* !CONTROLLABLE_HPP_ */
