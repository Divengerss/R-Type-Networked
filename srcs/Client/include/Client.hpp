#ifndef R_TYPE_CLIENT_HPP
#define R_TYPE_CLIENT_HPP

#ifdef _WIN32
    #define _WIN32_WINNT 0x0601
#endif /* !_WIN32 */

namespace rtype
{
    class Game
    {
        public:
            Game() = default;
            ~Game() = default;

            void runGame()
            {
                while (true) {}
            }

            private:
    };
} // namespace rtype

#endif //R_TYPE_CLIENT_HPP
