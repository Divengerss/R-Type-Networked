/*
** EPITECH PROJECT, 2023
** Sans titre(Espace de travail)
** File description:
** Collider
*/

#ifndef COLLIDER_HPP_
#define COLLIDER_HPP_

#include <vector>

class Collider {
    public:
        Collider(std::vector<std::size_t> collisions) {
            _collisions = collisions;
        };

    std::vector<std::size_t> _collisions;
};

#endif /* !COLLIDER_HPP_ */
