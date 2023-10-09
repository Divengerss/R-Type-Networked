/*
** EPITECH PROJECT, 2023
** Sans titre(Espace de travail)
** File description:
** Registry
*/

#include "Registry.hpp"

int main() {
    Registry reg;
    reg.register_component<Position>();
    Entity entity = reg.spawn_entity();
    const Position pos = Position();
    std::cout << entity() << std::endl;
    reg.add_component<Position>(entity, pos);
    reg.get_components<Position>();
}