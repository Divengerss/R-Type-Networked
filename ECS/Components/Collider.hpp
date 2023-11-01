/*
** EPITECH PROJECT, 2023
** Sans titre(Espace de travail)
** File description:
** Collider
*/

#ifndef COLLIDER_HPP_
#define COLLIDER_HPP_

class Collider {
    public:
        Collider(std::vector<std::size_t> entities) {
            _with = entities;
        };

    std::vector<std::size_t> _with;

    protected:
    private:
};

#endif /* !COLLIDER_HPP_ */
