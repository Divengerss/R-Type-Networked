/*
** EPITECH PROJECT, 2023
** Sans titre(Espace de travail)
** File description:
** ISystem
*/

#ifndef ISYSTEM_HPP_
#define ISYSTEM_HPP_

class Registry;
class ISystem {
    public:
        virtual void runSystem(Registry &r) = 0;

    protected:
    private:
};

#endif /* !ISYSTEM_HPP_ */
