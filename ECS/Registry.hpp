/*
** EPITECH PROJECT, 2023
** Sans titre(Espace de travail)
** File description:
** Registry
*/

#ifndef REGISTRY_HPP_
#define REGISTRY_HPP_

#include "SparseArray.hpp"
#include "Entity.hpp"
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <list>

#include <iostream>
#include <optional>

// Debug register_component, est-ce que le type_index et le vector se changent comme tu veux ?
// Debug quand tu handle recieve, le registry est-ce qu'il est good ? et quand t'appelles les fonctions qui touchent aux vectors est-ce que c'est good aussi ?
// Debug handleECSComponent

class Registry
{
public:
    Registry() = default;

    template <class Component>
    sparse_array<Component> &register_component()
    {
        std::type_index type_index(typeid(Component));
        _components_arrays.insert(std::pair<std::type_index, std::any>(type_index, sparse_array<Component>()));
        _erase_functions[type_index] = [this](Registry &registry, Entity const &entity)
        {
            registry.get_components<Component>().erase(entity());
        };
        _add_functions[type_index] = [this](Registry &registry)
        {
            registry.get_components<Component>().push_back(std::nullopt);
        };
        return std::any_cast<sparse_array<Component> &>(_components_arrays[type_index]);
    }

    template <class Component>
    sparse_array<Component> &get_components()
    {
        auto it = _components_arrays.find(typeid(Component));
        return std::any_cast<sparse_array<Component> &>(it->second);
    }

    template <class Component>
    sparse_array<Component> const &get_components() const
    {
        auto it = _components_arrays.find(typeid(Component));
        return std::any_cast<const sparse_array<Component> &>(it->second);
    }

    template <class Component>
    Component &get_component(Entity const &e)
    {
        sparse_array<Component> &array = get_components<Component>();
        std::optional<Component> &opt = array[e()];
        return *opt;
    }

    template <class Component>
    Component const &get_component(Entity const &e) const
    {
        sparse_array<Component> const &array = get_components<Component>();
        std::optional<Component> const &opt = array[e()];
        return *opt;
    }

    // #########################################################
    // #                 ENTITY MANAGEMENT                     #
    // #########################################################

    Entity spawn_entity()
    {
        if (_empty_entities.empty())
        {
            for (auto it : _add_functions)
                it.second(*this);
            Entity entity(entity_number);
            entity_number++;
            return entity;
        }
        else
        {
            Entity entity(_empty_entities.front());
            _empty_entities.pop_front();
            return entity;
        }
    }

    Entity &entity_from_index(std::size_t idx);

    template <typename Component>
    bool entity_has_component(Entity const &e) const
    {
        sparse_array<Component> const &array = get_components<Component>();
        return array.has_value(e());
    }

    int get_entity_number() const
    {
        return entity_number;
    }

    void kill_entity(Entity const &e)
    {
        if (static_cast<int>(e()) >= entity_number || static_cast<int>(e()) < 0)
            return;
        for (int i : _empty_entities)
            if (i == static_cast<int>(e()))
                return;

        for (auto it : _erase_functions)
            it.second(*this, e);
        _empty_entities.push_back(static_cast<int>(e()));
        _empty_entities.sort();
    }

    template <typename Component>
    typename sparse_array<Component>::reference_type add_component(Entity const &to, const Component &c)
    {
        sparse_array<Component> &array = get_components<Component>();
        array.insert_at(to(), c);
        return array[to()];
    }

    template <typename Component, typename... Params>
    typename sparse_array<Component>::reference_type emplace_component(Entity const &to, Params &&...p)
    {
        sparse_array<Component> &array = get_components<Component>();
        array.emplace_at(to, p...);
        return array[to()];
    }

    template <typename Component>
    void remove_component(Entity const &from)
    {
        sparse_array<Component> &array = get_components<Component>();
        array.erase(from());
    }

private:
    std::unordered_map<std::type_index, std::any> _components_arrays;
    std::unordered_map<std::type_index, std::function<void(Registry &, Entity const &)>> _erase_functions;
    std::unordered_map<std::type_index, std::function<void(Registry &)>> _add_functions;
    std::list<int> _empty_entities;
    int entity_number = 0;
};

#endif /* !REGISTRY_HPP_ */
