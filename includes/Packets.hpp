#ifndef PACKETS_HPP
#define PACKETS_HPP

#include <cstdint>
#include <array>
#include <string>
#include <cstring>

#define ACCEPTED    0x01U
#define REJECTED    0x02U
#define NEW_CLIENT  0x03U
#define LOSE_CLIENT 0x04U

#define REQUEST     0x00U

#define UUID_SIZE   0x24UL

#pragma pack(push, 1)

namespace packet
{
    enum packetTypes
    {
        PLACEHOLDER,
        CONNECTION_REQUEST,
        DISCONNECTION_REQUEST,
        CLIENT_STATUS,
    };

    struct packetHeader
    {
        packetTypes type;
        std::uint16_t dataSize;

        packetHeader() : type(PLACEHOLDER), dataSize(0) {}
        packetHeader(packetTypes type, std::uint16_t dataSize) : type(type), dataSize(dataSize) {}
    };

    struct connectionRequest
    {
        std::uint8_t status;
        std::array<std::uint8_t, UUID_SIZE> uuid;

        connectionRequest() : status(REQUEST)
        {
            std::memset(&uuid, 0, UUID_SIZE);
        }
        connectionRequest(uint8_t status, const std::string &cliUuid) : status(status)
        {
            std::memmove(&uuid, cliUuid.data(), UUID_SIZE);
        }
    };

    struct disconnectionRequest
    {
        std::uint8_t status;
        std::array<std::uint8_t, UUID_SIZE> uuid;

        disconnectionRequest(const std::string &cliUuid) : status(REQUEST)
        {
            std::memmove(&uuid, cliUuid.data(), UUID_SIZE);
        }
        disconnectionRequest(uint8_t status) : status(status), uuid({})
        {
        }
    };

    struct clientStatus
    {
        std::uint8_t status;
        std::array<std::uint8_t, UUID_SIZE> uuid;

        clientStatus() : status(LOSE_CLIENT) {}
        clientStatus(const std::string &cliUuid) : status(LOSE_CLIENT)
        {
            std::memmove(&uuid, cliUuid.data(), UUID_SIZE);
        }
        clientStatus(const std::string &cliUuid, std::uint8_t status) : status(status)
        {
            std::memmove(&uuid, cliUuid.data(), UUID_SIZE);
        }
    };
};

#pragma pack(pop)
#endif //PACKETS_HPP
