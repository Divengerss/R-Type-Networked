/*
** EPITECH PROJECT, 2023
** Sans titre(Espace de travail)
** File description:
** Entity
*/

#ifndef ENTITY_HPP_
#define ENTITY_HPP_
#include "cstddef"

class Entity {
    public:
        explicit Entity(size_t size) {};
        operator size_t() const {
            return _size;
        };
    protected:
    private:
        size_t _size;
};

#endif /* !ENTITY_HPP_ */
