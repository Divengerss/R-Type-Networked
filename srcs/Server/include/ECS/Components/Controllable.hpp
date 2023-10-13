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
        Controllable(bool playerId = false) {
            _playerId = playerId;
        };

    std::string _playerId;
    protected:
    private:
};

#endif /* !CONTROLLABLE_HPP_ */
