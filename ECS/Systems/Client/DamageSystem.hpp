/*
** EPITECH PROJECT, 2023
** Sans titre(Espace de travail)
** File description:
** DamageSystem
*/

#ifndef DAMAGESYSTEM_HPP_
#define DAMAGESYSTEM_HPP_

#include "Damaging.hpp"
#include "Destroyable.hpp"
#include "Collider.hpp"
#include "ISystem.hpp"

#include "Registry.hpp"

class DamageSystem : public ISystem
{
public:
    void runSystem(Registry &r)
    {
        auto const &damages = r.get_components<Damaging>();
        auto const &destroyable = r.get_components<Destroyable>();
        auto &collider = r.get_components<Collider>();
        for (std::size_t i = 0; i < destroyable.size(); ++i) {
            auto col = collider[i];
            if (col) {
                for (std::size_t j = 0; j < col->_with.size(); ++j) {
                    if (destroyable[i] && damages[col->_with[j]])
                        r.kill_entity(Entity(i));
                }
            }
        }
    }

protected:
private:
};

#endif /* !DAMAGESYSTEM_HPP_ */
