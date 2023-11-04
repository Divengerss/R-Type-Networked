/*
** EPITECH PROJECT, 2023
** Sans titre(Espace de travail)
** File description:
** BonusSystem
*/

#ifndef BONUSSYSTEM_HPP_
#define BONUSSYSTEM_HPP_

#include "Registry.hpp"
#include "Hitbox.hpp"
#include "Position.hpp"
#include "Pickup.hpp"
#include "Destroyable.hpp"
#include "ActiveBonus.hpp"
#include "ISystem.hpp"

class BonusSystem : public ISystem
{
public:
    void runSystem(Registry &r)
    {
        auto const hitboxes = r.get_components<Hitbox>();
        auto const positions = r.get_components<Position>();
        auto const pickups = r.get_components<Pickup>();
        auto bonuses = r.get_components<ActiveBonus>();
        auto destroyable = r.get_components<Destroyable>();

        for (size_t i = 0; i < positions.size(); ++i)
        {
            auto const &hb_player = hitboxes[i];
            auto const &pos_player = positions[i];
            auto &bonus = bonuses[i];
            if (pos_player && hb_player && bonus)
            {
                for (size_t j = 0; j < positions.size(); ++j)
                {
                    if (i == j)
                        continue;
                    auto const &hb_bonus = hitboxes[j];
                    auto const &pos_bonus = positions[j];
                    auto const &pickup = pickups[j];
                    auto &dest = destroyable[j];
                    if (pos_bonus && hb_bonus && pickup)
                    {
                        if (pos_player.value()._x < pos_bonus.value()._x + hb_bonus.value()._width &&
                            pos_player.value()._x + hb_player.value()._width > pos_bonus.value()._x &&
                            pos_player.value()._y < pos_bonus.value()._y + hb_bonus.value()._height &&
                            pos_player.value()._y + hb_player.value()._height > pos_bonus.value()._y)
                        {
                            // COLLISION
                            if (pickup.value()._positive == true)
                            {
                                switch (pickup.value()._bonusType)
                                {
                                case (BonusType::ForceBoost):
                                    bonus.value()._fb = ForceBoost::FORCEUP;
                                    break;
                                case (BonusType::SpeedBoost):
                                    bonus.value()._fb = SpeedBoost::SPEEDUP;
                                    break;
                                case (BonusType::ShootStyle):
                                    bonus.value()._ss = ShootStyle::THREEBULLET;
                                    break;
                                }
                            }
                            else
                            {
                                switch (pickup.value()._bonusType)
                                {
                                case (BonusType::ForceBoost):
                                    bonus.value()._fb = ForceBoost::FORCEDOWN;
                                    break;
                                case (BonusType::SpeedBoost):
                                    bonus.value()._fb = SpeedBoost::SPEEDDOWN;
                                    break;
                                case (BonusType::ShootStyle):
                                    bonus.value()._ss = ShootStyle::ONEBULLET;
                                    break;
                                }
                            }
                            dest.value()._hp = 0;
                        }
                    }
                }
            }
        }
    }

protected:
private:
};

#endif /* !BONUSSYSTEM_HPP_ */
