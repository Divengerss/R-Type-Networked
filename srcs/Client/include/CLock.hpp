/*
** EPITECH PROJECT, 2023
** r-type
** File description:
** CLock
*/

#ifndef CLOCK_HPP_
#define CLOCK_HPP_

#include <SFML/System.hpp>

class CLock {
    public:
        CLock();
        ~CLock();

    protected:
    private:
        sf::Clock _clock;
};

#endif /* !CLOCK_HPP_ */