#ifndef UUID_HPP_
#define UUID_HPP_

#include <string>
#include <cstdint>
#include <random>
#include <iomanip>
#include <sstream>

static constexpr std::size_t uuidSize = 0x24UL;

namespace uuid
{
    inline std::uint32_t generateRandomInt()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<std::uint32_t> uid;
        return uid(gen);
    }

    inline std::string generateUUID()
    {
        std::stringstream ss;
        
        for (std::uint8_t i = 0U; i < 4U; ++i) {
            std::uint32_t randInt = generateRandomInt();
            ss << std::hex << std::setw(8) << std::setfill('0') << randInt;
            if (i < 3U)
                ss << "-";
        }
        return ss.str();
    }
}

#endif /* !UUID_HPP_ */