/*
** EPITECH PROJECT, 2023
** R-Type
** File description:
** Engine
*/

#ifndef ENGINE_HPP_
#define ENGINE_HPP_

#include "Registry.hpp"
#include "Position.hpp"
#include "Controllable.hpp"
#include "Hitbox.hpp"
#include "Velocity.hpp"
#include "Destroyable.hpp"
#include "Tag.hpp"
#include "Score.hpp"

namespace core
{
    class Engine
    {
    public:
        Engine() = delete;
        Engine(Registry &ecs) : _ecs(ecs) {}
        ~Engine() = default;

        std::tuple<float, float> addPlayer(const std::string &clientUUID, std::size_t onlineNb)
        {
            float posX = 30.0f;
            float posY = 250.0f * onlineNb;
            Position position(posX, posY);
            Controllable ctrl(clientUUID);
            Hitbox hb(99, 51);
            Entity entity = _ecs.spawn_entity();
            _ecs.add_component<Position>(entity, position);
            _ecs.add_component<Velocity>(entity, 3);
            _ecs.add_component<Controllable>(entity, ctrl);
            _ecs.add_component<Hitbox>(entity, hb);
            _ecs.add_component<Destroyable>(entity, 1);
            _ecs.add_component<Tag>(entity, {TagEnum::PLAYER});
            return std::make_tuple(posX, posY);
        }

        void removePlayer(const std::string &clientUUID)
        {
            for (auto &component : _ecs.get_components<Controllable>())
            {
                std::string playId(uuidSize, 0);
                std::memmove(playId.data(), &component->_playerId, uuidSize);
                if (component.has_value() && !std::strcmp(playId.c_str(), clientUUID.c_str()))
                {
                    _ecs.get_component<Destroyable>(Entity(_ecs.get_components<Controllable>().get_index(component)))._hp = 0;
                }
            }
        }

        void affectControllable(const std::string &clientUUID, int keyCode)
        {
            auto &conts = _ecs.get_components<Controllable>();
            for (auto &cont : conts)
            {
                std::string playId(uuidSize, 0);
                std::memmove(playId.data(), &cont->_playerId, uuidSize);
                if (cont && !std::strcmp(playId.c_str(), clientUUID.c_str()))
                {
                    cont->latestInput = keyCode;
                }
            }
        }

    private:
        Registry &_ecs;
    };
}

#endif /* !ENGINE_HPP_ */
