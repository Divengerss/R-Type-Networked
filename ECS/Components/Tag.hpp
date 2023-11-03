/*
** EPITECH PROJECT, 2023
** Sans titre(Espace de travail)
** File description:
** Tag
*/

#ifndef TAG_HPP_
#define TAG_HPP_

enum TagEnum
{
    PLAYER,
    ENEMY,
    BULLET,
    NOTAG
};

class Tag
{
public:
    Tag(TagEnum tag)
    {
        _tag = tag;
    };

    int _tag;
};

#endif /* !TAG_HPP_ */
