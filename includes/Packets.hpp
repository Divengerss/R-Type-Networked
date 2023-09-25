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

    struct placeholder
    {
        std::uint8_t type;

        placeholder() : type(PLACEHOLDER) {}
    };

    struct connectionRequest
    {
        std::uint8_t type;
        std::uint8_t status;
        std::array<std::uint8_t, UUID_SIZE> uuid;

        connectionRequest() : type(CONNECTION_REQUEST), status(REQUEST)
        {
            std::memset(&uuid, 0, UUID_SIZE);
        }
        connectionRequest(uint8_t status, const std::string &cliUuid) : type(CONNECTION_REQUEST), status(status)
        {
            std::memcpy(&uuid, cliUuid.data(), UUID_SIZE);
        }
    };

    struct disconnectionRequest
    {
        std::uint8_t type;
        std::uint8_t status;
        std::array<std::uint8_t, UUID_SIZE> uuid;

        disconnectionRequest(const std::string &cliUuid) : type(DISCONNECTION_REQUEST), status(REQUEST)
        {
            std::memcpy(&uuid, cliUuid.data(), UUID_SIZE);
        }
        disconnectionRequest(uint8_t status) : type(DISCONNECTION_REQUEST), status(status) {}
    };
};

#pragma pack(pop)
#endif //PACKETS_HPP
