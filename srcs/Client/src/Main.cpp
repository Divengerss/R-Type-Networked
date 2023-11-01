#include "Client.hpp"
#include "Registry.hpp"
#include "Client/GameSystem.hpp"
#include "Client/PositionSystem.hpp"
#include "Client/TextureSystem.hpp"
#include "Client/DamageSystem.hpp"
#include "Client/CollidingSystem.hpp"
#include "Client/InputSystem.hpp"


int main()
{
    utils::ParseCFG config(utils::getCurrDir() + clientConfigFilePath.data());
    asio::io_context ioContext;
    Registry reg;
    DamageSystem ds;
    GameSystem gs;
    InputSystem is;
    PositionSystem ps;
    TextureSystem ts;
    CollidingSystem cs;
    ISystem *igs = &gs;
    ISystem *ips = &ps;
    ISystem *ics = &cs;
    ISystem *ids = &ds;
    ISystem *iis = &is;
    ISystem *its = &ts;
    reg.add_systems({igs, ips, its, iis, ics, ids});
    try {
        std::string host = config.getData<std::string>("host");
        std::string port = config.getData<std::string>("port");
        net::Client client(ioContext, host, port, reg);
        rtype::Game game(client, reg);
        game.runGame();
    } catch (const Error &e) {
        std::cerr << e.what() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
