/*
** EPITECH PROJECT, 2023
** Sans titre(Espace de travail)
** File description:
** Component
*/

#ifndef COMPONENT_HPP_
#define COMPONENT_HPP_

#include <optional>
#include <vector>
#include <any>
#include <cstdint>
#include <iostream>
#include <limits>

template <class Component> // You can also mirror the definition of std :: vector ,that takes an additional allocator.
class sparse_array
{
public:
    using value_type = std::optional<Component>;
    using reference_type = value_type &;
    using const_reference_type = value_type const &;
    using container_t = std::vector<value_type>; // optionally add your allocator
    using size_type = typename container_t::size_type;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t ::const_iterator;

    sparse_array() {};
    sparse_array(const sparse_array &other)
    {
        for (const auto &element : other._data)
        {
            if (element.has_value())
                _data.emplace_back(*element);
            else
                _data.emplace_back(std::nullopt);
        }
    }; // copy constructor
    sparse_array(sparse_array &&other) noexcept
    {
        _data = std::move(other._data);
    }; //move constructor
    ~sparse_array()
    {
        _data.clear();
    };
    sparse_array &operator=(const sparse_array &other)
    {
        if (this == &other)
        {
            return *this;
        }
        _data = other._data;
        return *this;
    }
    sparse_array &operator=(sparse_array &&other) noexcept
    {
        _data = std::move(other._data);
        return *this;
    }; // move assignment operator
    reference_type operator[](size_t idx)
    {
        return _data[idx];
    };
    const_reference_type operator[](size_t idx) const
    {
        return _data[idx];
    };
    iterator begin()
    {
        return _data.begin();
    };
    const_iterator begin() const
    {
        return _data.begin();
    }
    const_iterator cbegin() const
    {
        return _data.cbegin();
    };
    iterator end()
    {
        return _data.end();
    };
    const_iterator end() const
    {
        return _data.end();
    };
    const_iterator cend() const
    {
        return _data.cend();
    };
    size_type size() const
    {
        return _data.size();
    };

    reference_type insert_at(size_type const &pos, Component const &value)
    {
        if(_data.size() != 0) {
            auto it = _data.erase(_data.begin() + pos);
            it = _data.insert(it, value);
        } else {
            auto it = _data.begin();
            it = _data.insert(it, value);
        }
        return *std::next(_data.begin(), pos);
    };

    reference_type insert_at(size_type pos, Component &&value)
    {
        _data.erase(_data.begin() + pos);
        return _data.insert(_data.begin() + pos, std::forward<Component>(value));
    };

    template <class... Params>
    reference_type emplace_at(size_type pos, Params &&...data)
    {
        _data.erase(_data.begin() + pos);
        return _data.emplace(_data.begin() + pos, data...);
    }

    reference_type push_back(std::optional<Component> &&value) {
        _data.push_back(value);
        return (_data.back());
    }

    void erase(size_type pos)
    {
        _data[pos] = std::nullopt;
    };

    size_type get_index(value_type const &value) const
    {
        for (size_type i = 0UL; i < _data.size(); ++i)
            if (std::addressof(value) == std::addressof(_data[i]))
                return i;
        return std::numeric_limits<std::uint64_t>::max();
    };

private:
    container_t _data;
};

#endif /* !COMPONENT_HPP_ */