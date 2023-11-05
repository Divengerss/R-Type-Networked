#pragma once

#include "Registry.hpp"
#include "Texture.hpp"
#include "Tag.hpp"
#include "Scale.hpp"

class TextureSystem
{
public:
    TextureSystem() = default;
    ~TextureSystem() = default;

    void run(Registry& reg, int &createdPlayers)
    {
        for (int i = 0; i < reg.get_entity_number(); ++i) {
            if (reg.entity_has_component<Texture>(Entity(i)))
                continue;

            if (reg.entity_has_component<Tag>(Entity(i))) {
                auto &tag = reg.get_component<Tag>(Entity(i));

                switch (tag._tag) {
                case TagEnum::PLAYER:
                    reg.add_component<Texture>(Entity(i), {"player", 66, createdPlayers * 18, 33, 17});
                    reg.add_component<Scale>(Entity(i), {3, 3});
                    createdPlayers++;
                    break;
                case TagEnum::ENEMY:
                    reg.add_component<Texture>(Entity(i), {"enemy", 233, 0, 33, 36});
                    reg.add_component<Scale>(Entity(i), {3, 3});
                    break;
                case TagEnum::BULLET:
                    reg.add_component<Texture>(Entity(i), {"bullet", 185, 0, 25, 25});
                    reg.add_component<Scale>(Entity(i), {3, 3});
                    break;
                }
            }
        }
    }
};
