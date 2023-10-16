/*
** EPITECH PROJECT, 2023
** r-type
** File description:
** MovementPattern
*/

#ifndef MOVEMENTPATTERN_HPP_
#define MOVEMENTPATTERN_HPP_

enum MovementPatterns{
    STRAIGHTLEFT,
    STRAIGHTRIGHT,
    SINUS,
    CIRCLE,
    NONE,
};

class MovementPattern {
    public:
        MovementPattern(MovementPatterns movementPattern) {
            _movementPattern = movementPattern;
        };

        MovementPatterns _movementPattern;
        int _baseHeight;
};

#endif /* !MOVEMENTPATTERN_HPP_ */