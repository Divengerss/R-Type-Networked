#ifndef PACKETS_HPP
#define PACKETS_HPP

#include <cstdint>

#define ACCEPTED  0x01
#define REJECTED  0x02

#define REQUEST   0x00

#pragma pack(push, 1)

namespace packet
{
    struct connectionRequest
    {
        std::uint8_t type;
        std::uint8_t status;

        connectionRequest() : type(0x01), status(REQUEST) {}
        connectionRequest(uint8_t status) : type(0x01), status(status) {}
    };
};

#pragma pack(pop)
#endif //PACKETS_HPP
