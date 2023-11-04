/*
** EPITECH PROJECT, 2023
** R-Type
** File description:
** Network
*/

#include "Network.hpp"
#include "CFGParser.hpp"
#include "Position.hpp"
#include "Controllable.hpp"
#include "Hitbox.hpp"
#include "Velocity.hpp"
#include "Destroyable.hpp"
#include "Uuid.hpp"

net::Network::Network() :
    _ioContext(asio::io_context()), _ioService(asio::io_service()),
    _host(defaultHost), _port(defaultPort),
    _socket(_ioContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), defaultPort)),
    _logs(Log(utils::getCurrDir() + serverLogFile.data())),
    _clients({})
{
    _logs.logTo(logInfo.data(), "Starting up server...");
    utils::ParseCFG config(utils::getCurrDir() + serverConfigFilePath.data());
    try {
        _host = config.getData<std::string>("host");
        _port = static_cast<std::uint16_t>(std::stoi(config.getData<std::string>("port")));
        _logs.logTo(logInfo.data(), "Retrieved configs successfully");
    } catch (const Error &e) {
        std::string err = e.what();
        setConnection(defaultHost.data(), defaultPort);
        _logs.logTo(logErr.data(), "Failed to retrieve configs: " + err);
        _logs.logTo(logWarn.data(), "Default host and ports applied");
    }
    _socket.set_option(asio::ip::udp::socket::reuse_address(true));
    try {
        _socket = asio::ip::udp::socket(_ioContext, asio::ip::udp::endpoint(asio::ip::make_address(_host), _port));
    } catch (const std::system_error &e) {
        std::string err = e.what();
        _logs.logTo(logErr.data(), "Failed to start the server: " + err);
        throw std::runtime_error("socket");
    } catch (const std::exception &e) {
        std::string err = e.what();
        _logs.logTo(logErr.data(), "Failed to start the server: " + err);
        throw std::runtime_error("bind");
    }
    _threadPool.emplace_back([&]() {
        _logs.logTo(logInfo.data(), "Starting network execution context");
        _ioContext.run();
    });
    _threadPool.emplace_back([&]() {
        _logs.logTo(logInfo.data(), "Starting network execution service");
        _ioService.run();
    });
    std::signal(SIGINT, signalHandler);
    receive();
}

net::Network::~Network()
{
    stopServer();
    for (std::thread &thread : _threadPool) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    _logs.logTo(logInfo.data(), "Network has been shut down successfully.");
}

void net::Network::receiveCallback(const asio::error_code &errCode, std::size_t bytesReceived)
{
    if (bytesReceived == 0UL) {
        _logs.logTo(logInfo.data(), "The received packet was empty. " + errCode.message());
    } else if (errCode)
        _logs.logTo(logErr.data(), "Error receiving packet: " + errCode.message());
    receive();
}

void net::Network::receive()
{
    _packet.fill(0);
    _socket.async_receive_from(
        asio::buffer(_packet, _packet.size()),
        _serverEndpoint,
        std::bind(&Network::receiveCallback, this, std::placeholders::_1, std::placeholders::_2)
    );
}

void net::Network::startServer()
{
    setServerInstance(this);
    _logs.logTo(logInfo.data(), "Network initialized successfully");
    _logs.logTo(logInfo.data(), "Listening at " + _host + " on port " + std::to_string(_port));
    receive();
}

void net::Network::stopServer()
{
    try {
        _socket.close();
    } catch (const std::system_error &e) {
        std::string err = e.what();
        _logs.logTo(logWarn.data(), "Failed to close the socket:");
        _logs.logTo(logWarn.data(), "    " + err);
    }
    _ioContext.stop();
    _ioService.stop();
}

std::string net::Network::addClient(std::uint64_t roomId)
{
    std::string cliUuid = uuid::generateUUID();
    _clients.emplace_back(Client(cliUuid, _serverEndpoint, roomId));
    return cliUuid;
}

bool net::Network::removeClient(const std::string &uuid)
{
    std::vector<Client>::iterator it = std::remove_if(_clients.begin(), _clients.end(),
        [&](const Client &client) {
            return client.getUuid() == uuid.data();
        }
    );

    if (it != _clients.end()) {
        _clients.erase(it, _clients.end());
        return true;
    }
    return false;
}