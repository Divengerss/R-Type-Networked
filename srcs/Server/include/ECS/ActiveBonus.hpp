/*
** EPITECH PROJECT, 2023
** r-type
** File description:
** ActiveBonus
*/

#ifndef ACTIVEBONUS_HPP_
#define ACTIVEBONUS_HPP_

enum ShootStyle{
    ONEBULLET,
    TWOBULLET,
    THREEBULLET,
};

enum SpeedBoost{
    SPEEDUP,
    SPEEDDOWN,
    SPEEDNORMAL,
};

enum ForceBoost{
    FORCEUP,
    FORCEDOWN,
    FORCENORMAL,
};

class ActiveBonus {
    public:
        ActiveBonus(ShootStyle, SpeedBoost, ForceBoost) {
        };

    protected:
    private:
};

#endif /* !ACTIVEBONUS_HPP_ */
