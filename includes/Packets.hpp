#ifndef PACKETS_HPP
#define PACKETS_HPP

#include <cstdint>
#include <array>
#include <string>
#include <cstring>
#include "Uuid.hpp"

#pragma pack(push, 1)

namespace packet
{
    enum packetTypes : std::uint8_t
    {
        NONE,
        PLACEHOLDER,
        CONNECTION_REQUEST,
        DISCONNECTION_REQUEST,
        CLIENT_STATUS,
        FORCE_DISCONNECT,
        ECS_VELOCITY,
        ECS_POSITION,
        ECS_HITBOX,
        ECS_CONTROLLABLE,
        KEYBOARD_EVENT,
        ECS_DAMAGES,
        ECS_DESTROYABLE,
        ECS_MOVEMENTPATTERN,
        ECS_SCORE
    };

    enum packetStatus : std::uint8_t
    {
        REQUEST,
        ACCEPTED,
        REJECTED,
        NEW_CLIENT,
        LOSE_CLIENT,
    };

    struct packetHeader
    {
        packetTypes type;
        std::uint16_t dataSize;

        packetHeader() : type(PLACEHOLDER), dataSize(0U) {}
        packetHeader(packetTypes type, std::uint16_t dataSize) : type(type), dataSize(dataSize) {}
    };

    struct connectionRequest
    {
        std::uint8_t status;
        std::array<std::uint8_t, uuidSize> uuid;
        std::size_t connectedNb;

        connectionRequest() : status(REQUEST), connectedNb(0)
        {
            std::memset(&uuid, 0, uuidSize);
        }
        connectionRequest(uint8_t status, const std::string &cliUuid) : status(status), connectedNb(0)
        {
            std::memmove(&uuid, cliUuid.data(), uuidSize);
        }
        connectionRequest(uint8_t status, const std::string &cliUuid, std::size_t connectedCount) : status(status), connectedNb(connectedCount)
        {
            std::memmove(&uuid, cliUuid.data(), uuidSize);
        }
    };

    struct disconnectionRequest
    {
        std::uint8_t status;
        std::array<std::uint8_t, uuidSize> uuid;
        std::size_t connectedNb;

        disconnectionRequest(const std::string &cliUuid) : status(REQUEST)
        {
            std::memmove(&uuid, cliUuid.data(), uuidSize);
            connectedNb = 0UL;
        }
        disconnectionRequest(uint8_t status) : status(status), uuid({})
        {
            connectedNb = 0UL;
        }
        disconnectionRequest(const std::string &cliUuid, std::size_t connectedCount) : status(REQUEST)
        {
            std::memmove(&uuid, cliUuid.data(), uuidSize);
            connectedNb = connectedCount;
        }
        disconnectionRequest(uint8_t status, std::size_t connectedCount) : status(status), uuid({})
        {
            connectedNb = connectedCount;
        }
    };

    struct clientStatus
    {
        std::uint8_t status;
        std::array<std::uint8_t, uuidSize> uuid;
        float posX;
        float posY;
        std::size_t connectedNb;

        clientStatus() : status(LOSE_CLIENT), posX(0.0f), posY(0.0f), connectedNb(0) {}
        clientStatus(const std::string &cliUuid) : status(LOSE_CLIENT), posX(0.0f), posY(0.0f), connectedNb(0)
        {
            std::memmove(&uuid, cliUuid.data(), uuidSize);
        }
        clientStatus(const std::string &cliUuid, std::uint8_t status) : status(status), posX(0.0f), posY(0.0f), connectedNb(0)
        {
            std::memmove(&uuid, cliUuid.data(), uuidSize);
        }
        clientStatus(const std::string &cliUuid, std::uint8_t status, float x, float y) : status(status), posX(x), posY(y), connectedNb(0)
        {
            std::memmove(&uuid, cliUuid.data(), uuidSize);
        }
        clientStatus(const std::string &cliUuid, std::uint8_t status, float x, float y, std::size_t connectedCount) : status(status), posX(x), posY(y), connectedNb(connectedCount)
        {
            std::memmove(&uuid, cliUuid.data(), uuidSize);
        }
    };

    struct keyboardEvent
    {
        std::uint8_t _status;
        std::array<std::uint8_t, uuidSize> uuid;
        int keyCode;
        keyboardEvent() : _status(0), keyCode(-1) {};
        keyboardEvent(const std::string &cliUuid, std::uint8_t status, int key) {
            std::memmove(&uuid, cliUuid.data(), uuidSize);
            _status = status;
            keyCode = key;
        };
    };
}

#pragma pack(pop)
#endif //PACKETS_HPP
