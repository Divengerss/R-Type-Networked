#ifndef PACKETS_HPP
#define PACKETS_HPP

#include <cstdint>
#include <array>
#include <string>
#include <cstring>

static constexpr std::size_t uuidSize = 0x24UL;

#pragma pack(push, 1)

namespace packet
{
    enum packetTypes : std::uint8_t
    {
        PLACEHOLDER,
        CONNECTION_REQUEST,
        DISCONNECTION_REQUEST,
        CLIENT_STATUS,
        FORCE_DISCONNECT,
        TEST_ECS
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

        connectionRequest() : status(REQUEST)
        {
            std::memset(&uuid, 0, uuidSize);
        }
        connectionRequest(uint8_t status, const std::string &cliUuid) : status(status)
        {
            std::memmove(&uuid, cliUuid.data(), uuidSize);
        }
    };

    struct disconnectionRequest
    {
        std::uint8_t status;
        std::array<std::uint8_t, uuidSize> uuid;

        disconnectionRequest(const std::string &cliUuid) : status(REQUEST)
        {
            std::memmove(&uuid, cliUuid.data(), uuidSize);
        }
        disconnectionRequest(uint8_t status) : status(status), uuid({})
        {
        }
    };

    struct clientStatus
    {
        std::uint8_t status;
        std::array<std::uint8_t, uuidSize> uuid;

        clientStatus() : status(LOSE_CLIENT) {}
        clientStatus(const std::string &cliUuid) : status(LOSE_CLIENT)
        {
            std::memmove(&uuid, cliUuid.data(), uuidSize);
        }
        clientStatus(const std::string &cliUuid, std::uint8_t status) : status(status)
        {
            std::memmove(&uuid, cliUuid.data(), uuidSize);
        }
    };
};

#pragma pack(pop)
#endif //PACKETS_HPP
