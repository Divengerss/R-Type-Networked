#ifndef R_TYPE_CLIENT_HPP
#define R_TYPE_CLIENT_HPP

#ifdef _WIN32
    #define _WIN32_WINNT 0x0601
#endif /* !_WIN32 */

#include "Network.hpp"

namespace rtype
{
    class Game
    {
        public:
            Game() = default;
            Game(net::Client &client) : _client(client) {}
            ~Game() = default;

            void runGame()
            {
                while (_client.isSocketOpen())
                {

                }
            }

            private:
                net::Client &_client;
    };
} // namespace rtype

#endif //R_TYPE_CLIENT_HPP
