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
        ActiveBonus(int ss = ShootStyle::ONEBULLET, int sb = SpeedBoost::SPEEDNORMAL, int fb = ForceBoost::FORCENORMAL) {
            _ss = ss;
            _fb = fb;
            _sb = sb;
        };

        int _ss;
        int _sb;
        int _fb;
};

#endif /* !ACTIVEBONUS_HPP_ */
