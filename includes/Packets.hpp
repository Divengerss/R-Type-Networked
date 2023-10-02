#ifndef PACKETS_HPP
#define PACKETS_HPP

#include <cstdint>

#define ACCEPTED  0x01
#define REJECTED  0x02

#define REQUEST   0x00

#define UUID_SIZE  0x24UL

#pragma pack(push, 1)

namespace packet
{
    enum packetTypes
    {
        PLACEHOLDER,
        CONNECTION_REQUEST,
        DISCONNECTION_REQUEST,
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
            std::memcpy(&uuid, cliUuid.data(), UUID_SIZE);
        }
    };

    struct disconnectionRequest
    {
        std::uint8_t status;
        std::array<packetTypes, UUID_SIZE> uuid;

        disconnectionRequest(const std::string &cliUuid) : status(REQUEST)
        {
            std::memcpy(&uuid, cliUuid.data(), UUID_SIZE);
        }
        disconnectionRequest(uint8_t status) : status(status), uuid({})
        {
        }
    };
};

#pragma pack(pop)
#endif //PACKETS_HPP
