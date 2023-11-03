/*
** EPITECH PROJECT, 2023
** R-Type
** File description:
** Room
*/

#ifndef ROOM_HPP_
#define ROOM_HPP_

#include <vector>
#include <string>
#include <cstdint>

namespace rtype
{
    class Room
    {
        public:
            Room() = delete;
            Room(std::uint64_t id, std::uint8_t maxSlots) : _id(id), _uuids({}), _maxSlots(maxSlots) {}
            ~Room() = default;

            std::uint64_t getId() const noexcept { return _id; };
            std::vector<std::string> &getClients() noexcept { return _uuids; };
            std::uint8_t getMaxSlots() const noexcept { return _maxSlots; };

            void setId(std::uint64_t id)
            {
                _id = id;
            }

            bool addClient(const std::string &clientUUID)
            {
                if (_uuids.size() < _maxSlots) {
                    _uuids.push_back(clientUUID);
                    return true;
                }
                return false;
            };

            void setMaxSlots(std::uint8_t max)
            {
                _maxSlots = max;
            };

        private:
            std::uint64_t _id;
            std::vector<std::string> _uuids;
            std::uint8_t _maxSlots;
    };
}

#endif /* !ROOM_HPP_ */
