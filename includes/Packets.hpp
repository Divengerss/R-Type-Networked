#ifndef PACKETS_HPP
#define PACKETS_HPP

#include <cstdint>

#define ACCEPTED  0x1
#define REJECTED  0x2

#pragma pack(push, 1)

namespace packet
{
    struct connectionRequest
    {
        std::uint8_t type;
        std::uint8_t status;

        connectionRequest() : type(0x01), status(0x00) {}
        connectionRequest(uint8_t status) : type(0x01), status(status) {}
    };
};

#pragma pack(pop)
#endif //PACKETS_HPP
