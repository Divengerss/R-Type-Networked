/*
** EPITECH PROJECT, 2023
** R-Type
** File description:
** Engine
*/

#ifndef ENGINE_HPP_
#define ENGINE_HPP_

#include <initializer_list>

#include "Registry.hpp"

namespace rtype
{
    class Engine
    {
        public:
            Engine();
            ~Engine();

            const Registry &getRegistry() const noexcept { return _ecs; }

            template <class... Component>
            void registerComponents() {
                (void) std::initializer_list<int> {
                    (static_cast<void>(_ecs.register_component<Component>()), 0)...
                };
            }

        private:
            Registry _ecs;
    };
}

#endif /* !ENGINE_HPP_ */
