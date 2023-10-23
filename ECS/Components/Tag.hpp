/*
** EPITECH PROJECT, 2023
** Sans titre(Espace de travail)
** File description:
** Tag
*/

#ifndef TAG_HPP_
#define TAG_HPP_

#include <string>

class Tag {
    public:
        Tag(std::string tag)
        {
            _tag = tag;
        }

        std::string _tag;
};

#endif /* !TAG_HPP_ */
