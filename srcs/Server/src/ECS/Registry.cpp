/*
** EPITECH PROJECT, 2023
** Sans titre(Espace de travail)
** File description:
** Registry
*/

#include "Registry.hpp"
#include "Position.hpp"
#include "Velocity.hpp"

int main() {
    Registry reg;
    reg.register_component<Position>();
    reg.register_component<Velocity>();
    reg.spawn_entity();
    auto positions = reg.get_components<Position>();
    auto velocities = reg.get_components<Velocity>();
    std::cout << positions.size() << " " << velocities.size() << std::endl;

}