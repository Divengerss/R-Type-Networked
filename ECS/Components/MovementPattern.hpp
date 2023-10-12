/*
** EPITECH PROJECT, 2023
** r-type
** File description:
** MovementPattern
*/

#ifndef MOVEMENTPATTERN_HPP_
#define MOVEMENTPATTERN_HPP_

enum MovementPatterns{
    STRAIGHT,
    SINUS,
    CIRCLE,
};

class MovementPattern {
    public:
        MovementPattern(MovementPatterns movementPattern) {
            _movementPattern = movementPattern;
        };

        MovementPatterns _movementPattern;
        int _baseHeight;
    protected:
    private:
};

#endif /* !MOVEMENTPATTERN_HPP_ */