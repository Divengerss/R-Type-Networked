#include "Game.hpp"
#include "Error.hpp"
#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <ctime>

int main()
{
    try
    {
        std::srand(static_cast<std::uint32_t>(std::time(nullptr)));
        rtype::loopSystem game;
        game.runNetwork();
        game.runGame();
    }
    catch (const Error &e)
    {
        std::cerr << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
    catch (const std::runtime_error &)
    {
        std::exit(EXIT_FAILURE);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}
