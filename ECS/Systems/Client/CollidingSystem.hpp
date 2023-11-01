/*
** EPITECH PROJECT, 2023
** Sans titre(Espace de travail)
** File description:
** CollidingSystem
*/

#ifndef COLLIDINGSYSTEM_HPP_
#define COLLIDINGSYSTEM_HPP_


#include "Collider.hpp"
#include "Position.hpp"
#include "Hitbox.hpp"
#include "Collider.hpp"
#include "ISystem.hpp"

#include "Registry.hpp"

class CollidingSystem : public ISystem
{
public:
    void runSystem(Registry &r)
    {
        auto const positions = r.get_components<Position>();
        auto const hitboxes = r.get_components<Hitbox>();
        auto colliders = r.get_components<Collider>();

        for (size_t i = 0; i < positions.size(); ++i)
        {
            auto const &hb_fir = hitboxes[i];
            auto const &pos_fir = positions[i];
            auto &col_fir = colliders[i];
            if (pos_fir && hb_fir)
            {
                for (size_t j = 0; j < positions.size(); ++j)
                {
                    if (i == j)
                        continue;
                    auto const &pos_sec = positions[j];
                    auto const &hb_sec = hitboxes[j];
                    auto &col_sec = colliders[j];
                    if (pos_sec && hb_sec &&
                        pos_fir->_x + hb_fir->_width >= pos_sec->_x &&  // hb_fir-> right edge past r2 left
                        pos_fir->_x <= pos_sec->_x + hb_sec->_width &&  // hb_fir-> left edge past hb_sec-> right
                        pos_fir->_y + hb_fir->_height >= pos_sec->_y && // r1 top edge past hb_sec-> bottom
                        pos_fir->_y <= pos_sec->_y + hb_sec->_height)   // r1 bottom edge past r2 top
                    {
                        if (!col_fir)
                            r.add_component<Collider>(Entity(i), {{j}});
                        else
                            col_fir->_with.push_back(j);
                        if (!col_sec)
                            r.add_component<Collider>(Entity(j), {{i}});
                        else
                            col_sec->_with.push_back(i);
                    }
                    else
                    {
                        if (col_fir && !col_fir->_with.empty() && std::find(col_fir->_with.begin(), col_fir->_with.end(), j) != col_fir->_with.end())
                            col_fir->_with.erase(std::find(col_fir->_with.begin(), col_fir->_with.end(), j));
                        if (col_sec && !col_sec->_with.empty() && std::find(col_sec->_with.begin(), col_sec->_with.end(), i) != col_sec->_with.end())
                            col_sec->_with.erase(std::find(col_sec->_with.begin(), col_sec->_with.end(), i));
                    }
                }
            }
        }
    }

protected:
private:
};

#endif /* !COLLIDINGSYSTEM_HPP_ */
