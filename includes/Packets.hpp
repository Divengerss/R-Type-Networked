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
    struct connectionRequest
    {
        std::uint8_t type;
        std::uint8_t status;
        std::array<std::uint8_t, UUID_SIZE> uuid;

        connectionRequest() : type(0x01), status(REQUEST)
        {
            std::memset(&uuid, 0, UUID_SIZE);
        }
        connectionRequest(uint8_t status, const std::string &cliUuid) : type(0x01), status(status)
        {
            std::memcpy(&uuid, cliUuid.data(), UUID_SIZE);
        }
    };

    struct disconnectionRequest
    {
        std::uint8_t type;
        std::uint8_t status;
        std::array<std::uint8_t, UUID_SIZE> uuid;

        disconnectionRequest(const std::string &cliUuid) : type(0x02), status(REQUEST)
        {
            std::memcpy(&uuid, cliUuid.data(), UUID_SIZE);
        }
        disconnectionRequest(uint8_t status) : type(0x02), status(status) {}
    };
};

#pragma pack(pop)
#endif //PACKETS_HPP
