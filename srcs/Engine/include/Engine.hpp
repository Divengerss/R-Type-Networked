/*
** EPITECH PROJECT, 2023
** R-Type
** File description:
** Engine
*/

#ifndef ENGINE_HPP_
#define ENGINE_HPP_

#include "Registry.hpp"

namespace rtype
{
    class Engine
    {
        public:
            Engine();
            ~Engine();

            const Registry &getRegistry() const noexcept { return _ecs; }

            template<class ... Component>
            void registerComponents(Component ... component)
            {
                _ecs.register_component<component...>();
                registerComponents(component...);
            }

        private:
            Registry _ecs;
    };
}

#endif /* !ENGINE_HPP_ */
