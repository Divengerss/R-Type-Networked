/*
** EPITECH PROJECT, 2023
** r-type
** File description:
** Pickup
*/

#ifndef PICKUP_HPP_
#define PICKUP_HPP_

enum BonusType {
    SpeedBoost,
    ForceBoost,
    ShootStyle
};
class Pickup {
    public:
        Pickup(BonusType bonusType = SpeedBoost, bool positive = true) {
            _bonusType = bonusType;
        };

        BonusType _bonusType;
        bool positive;
    protected:
    private:
};

#endif /* !PICKUP_HPP_ */
