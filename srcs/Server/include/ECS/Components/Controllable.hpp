/*
** EPITECH PROJECT, 2023
** Sans titre(Espace de travail)
** File description:
** Controllable
*/

#ifndef CONTROLLABLE_HPP_
#define CONTROLLABLE_HPP_
#include <string>

class Controllable {
    public:
        Controllable(const std::string &playerId) {
            _playerId = playerId;
        };

        std::string _playerId;
};

#endif /* !CONTROLLABLE_HPP_ */
