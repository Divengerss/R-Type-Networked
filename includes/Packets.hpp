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
        ECS_SCORE,
        ECS_COLLIDER,
        KEEP_CONNECTION,
        CREATE_ROOM,
        ROOM_LIST_REQUEST,
        ROOM_AVAILABLE,
        ROOM_CLOSED,
        JOINED_ROOM,
        LEFT_ROOM,
        TEXT_MESSAGE,
        ECS_TAG,
        ENTITY_KILLED,
        PING_REQUEST,
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
        std::uint64_t dataSize;
        bool compressed;
        std::size_t compressedSize;
        std::uint64_t roomId;

        packetHeader() : type(PLACEHOLDER), dataSize(0U), compressed(false), compressedSize(0UL), roomId(0UL) {}
        packetHeader(packetTypes type, std::uint64_t dataSize) : type(type), dataSize(dataSize), compressed(false), compressedSize(0UL), roomId(0UL) {}
        packetHeader(packetTypes type, std::uint64_t dataSize, bool compressed, std::size_t compressedSize, std::uint64_t roomId) : type(type), dataSize(dataSize), compressed(compressed), compressedSize(compressedSize), roomId(roomId) {}
    };

    struct connectionRequest
    {
        std::uint8_t status;
        std::array<std::uint8_t, uuidSize> uuid;
        std::size_t connectedNb;
        std::uint64_t roomId;
        bool createRoom;

        connectionRequest() : status(REQUEST), connectedNb(0UL), roomId(0UL), createRoom(false)
        {
            std::memset(&uuid, 0, uuidSize);
        }
        connectionRequest(std::uint64_t roomId) : status(REQUEST), connectedNb(0UL), roomId(roomId), createRoom(false)
        {
            std::memset(&uuid, 0, uuidSize);
        }
        connectionRequest(std::uint64_t roomId, bool createRoom) : status(REQUEST), connectedNb(0UL), roomId(roomId), createRoom(createRoom)
        {
            std::memset(&uuid, 0, uuidSize);
        }
        connectionRequest(uint8_t status, const std::string &cliUuid) : status(status), connectedNb(0UL), roomId(0UL), createRoom(false)
        {
            std::memmove(&uuid, cliUuid.data(), uuidSize);
        }
        connectionRequest(uint8_t status, const std::string &cliUuid, std::size_t connectedCount) : status(status), connectedNb(connectedCount), roomId(0UL), createRoom(false)
        {
            std::memmove(&uuid, cliUuid.data(), uuidSize);
        }
    };

    struct disconnectionRequest
    {
        std::uint8_t status;
        std::array<std::uint8_t, uuidSize> uuid;
        std::size_t connectedNb;
        std::uint64_t roomId;

        disconnectionRequest(const std::string &cliUuid, std::uint64_t roomId) : status(REQUEST), roomId(roomId)
        {
            std::memmove(&uuid, cliUuid.data(), uuidSize);
            connectedNb = 0UL;
        }
        disconnectionRequest(uint8_t status) : status(status), uuid({}), roomId(0UL)
        {
            connectedNb = 0UL;
        }
        disconnectionRequest(const std::string &cliUuid, std::size_t connectedCount, std::uint64_t roomId) : status(REQUEST), roomId(roomId)
        {
            std::memmove(&uuid, cliUuid.data(), uuidSize);
            connectedNb = connectedCount;
        }
        disconnectionRequest(uint8_t status, std::size_t connectedCount) : status(status), uuid({}), roomId(0UL)
        {
            connectedNb = connectedCount;
        }
    };

    struct entityKilledStatus
    {
        int entity;

        entityKilledStatus(int entity) : entity(entity){};
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
        std::uint64_t roomId;

        keyboardEvent() : _status(0), keyCode(-1), roomId(0UL)
        {
            std::memset(&uuid, 0, uuidSize);
        };
        keyboardEvent(const std::string &cliUuid, std::uint8_t status, int key, std::uint64_t roomId) : _status(status), keyCode(key), roomId(roomId)
        {
            std::memmove(&uuid, cliUuid.data(), uuidSize);
        };
    };

    struct keepConnection
    {
        std::array<std::uint8_t, uuidSize> uuid;

        keepConnection()
        {
            std::memset(&uuid, 0, uuidSize);
        }
        keepConnection(const std::string &clientUUID)
        {
            std::memmove(&uuid, clientUUID.data(), uuidSize);
        }
    };

    struct createRoom
    {
        std::uint64_t roomId;
        std::uint8_t maxSlots;

        createRoom(std::uint8_t maxSlots) : roomId(0UL), maxSlots(maxSlots) {}
        createRoom(std::uint64_t roomId, std::uint8_t maxSlots) : roomId(roomId), maxSlots(maxSlots) {}
    };

    struct roomListRequest
    {
        roomListRequest() {}
    };

    struct pingRequest
    {
        pingRequest() {}
    };

    struct roomAvailable
    {
        std::uint64_t roomId;
        std::uint8_t maxSlots;

        roomAvailable() : roomId(0UL), maxSlots(0UL) {}
        roomAvailable(std::uint64_t roomId, std::uint8_t maxSlots) : roomId(roomId), maxSlots(maxSlots) {}
    };

    struct roomClosed
    {
        std::uint64_t roomId;

        roomClosed() : roomId(0UL) {}
        roomClosed(std::uint64_t roomId) : roomId(roomId) {}
    };

    struct joinedRoom
    {
        std::array<std::uint8_t, uuidSize> uuid;
        std::uint64_t roomId;

        joinedRoom() : uuid({}), roomId(0UL) {}
        joinedRoom(const std::string &clientUUID, std::uint64_t roomId) : roomId(roomId)
        {
            std::memmove(&uuid, clientUUID.data(), uuidSize);
        }
    };

    struct leftRoom
    {
        std::array<std::uint8_t, uuidSize> uuid;
        std::uint64_t roomId;

        leftRoom() : uuid({}), roomId(0UL) {}
        leftRoom(const std::string &clientUUID, std::uint64_t roomId) : roomId(roomId)
        {
            std::memmove(&uuid, clientUUID.data(), uuidSize);
        }
    };

    struct textMessage
    {
        std::array<std::uint8_t, uuidSize> uuid;
        std::array<std::uint8_t, 256UL> message;
        std::size_t msgSize;

        textMessage(): uuid({}), msgSize(0UL)
        {
            std::memset(&message, 0, 256UL);
        }
        textMessage(const std::string &cliUuid, const std::string &msg) : msgSize(msg.length())
        {
            std::memset(&message, 0, 256UL);
            std::memmove(&uuid, cliUuid.data(), uuidSize);
            std::memmove(&message, msg.data(), msgSize);
        }
    };
}

#pragma pack(pop)
#endif // PACKETS_HPP
