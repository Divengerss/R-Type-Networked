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

net::Network::Network(Registry &ecs) :
    _ioContext(asio::io_context()), _ioService(asio::io_service()),
    _host(defaultHost), _port(defaultPort),
    _socket(_ioContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), defaultPort)),
    _logs(Log(utils::getCurrDir() + serverLogFile.data())),
    _clients({}), _game(core::Engine(ecs))
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
    for (std::thread &thread : _threadPool) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    _logs.logTo(logInfo.data(), "Network has been shut down successfully.");
}

void net::Network::handleConnectionRequest()
{
    std::string cliUuid = addClient();
    packet::connectionRequest response(packet::ACCEPTED, cliUuid, _clients.size());
    _logs.logTo(logInfo.data(), "New connection received: UUID [" + cliUuid + "]");
    packet::packetHeader header(packet::CONNECTION_REQUEST, sizeof(response));
    std::array<std::uint8_t, sizeof(header) + sizeof(response)> buffer;
    std::memmove(&buffer, &header, sizeof(header));
    std::memmove(&buffer[sizeof(header)], &response, sizeof(response));
    try {
        _socket.send_to(asio::buffer(&buffer, sizeof(buffer)), _serverEndpoint);
    } catch (const std::system_error &e) {
        std::string err = e.what();
        _logs.logTo(logWarn.data(), "Failed to send the response of packet type [" + std::to_string(header.type) + "]:");
        _logs.logTo(logWarn.data(), "    " + err);
    }
    std::tuple<int, int> positions = _game.addPlayer(cliUuid, getClients().size());
    packet::clientStatus cliStatus(cliUuid, packet::NEW_CLIENT, std::get<0>(positions), std::get<1>(positions), _clients.size());
    try {
        sendResponse(packet::CLIENT_STATUS, cliStatus);
    } catch (const std::system_error &e) {
        std::string err = e.what();
        _logs.logTo(logWarn.data(), "Failed to send the response of packet type [" + std::to_string(header.type) + "]:");
        _logs.logTo(logWarn.data(), "    " + err);
    }
}

void net::Network::handleDisconnectionRequest(packet::packetHeader &header)
{
    packet::disconnectionRequest request(packet::ACCEPTED, _clients.size());
    std::memmove(&request, &_packet[sizeof(header)], sizeof(request));
    std::string cliUuid(uuidSize, 0);
    std::memmove(cliUuid.data(), &request.uuid, uuidSize);
    removeClient(cliUuid);
    _logs.logTo(logInfo.data(), "Disconnection received from [" + cliUuid + "]");
    packet::clientStatus cliStatus(cliUuid, packet::LOSE_CLIENT, 0, 0, _clients.size());
    try {
        sendResponse(packet::CLIENT_STATUS, cliStatus);
    } catch (const std::system_error &e) {
        std::string err = e.what();
        _logs.logTo(logWarn.data(), "Failed to send the response of packet type [" + std::to_string(header.type) + "]:");
        _logs.logTo(logWarn.data(), "    " + err);
    }
    _game.removePlayer(cliUuid);
}

void net::Network::handleKeyboardEvent(packet::packetHeader &header)
{
    packet::keyboardEvent event;
    std::memmove(&event, &_packet[sizeof(header)], sizeof(event));
    std::string uuid (uuidSize, 0);
    std::memmove(uuid.data(), &event.uuid, uuidSize);
    _game.affectControllable(uuid, event.keyCode);
}

void net::Network::handleRequestStatus()
{
    packet::packetHeader header = *reinterpret_cast<packet::packetHeader*>(_packet.data());

    std::unordered_map<uint16_t, std::function<void()>> packetHandlers = {
        {packet::CONNECTION_REQUEST, [&]{ handleConnectionRequest(); }},
        {packet::DISCONNECTION_REQUEST, [&]{ handleDisconnectionRequest(header); }},
        {packet::KEYBOARD_EVENT, [&] {handleKeyboardEvent(header);}}
    };

    auto handlerIt = packetHandlers.find(header.type);
    if (handlerIt != packetHandlers.end()) {
        handlerIt->second();
    } else {
        _logs.logTo(logWarn.data(), "Unknown packet");
    }
}

void net::Network::receiveCallback(const asio::error_code &errCode, std::size_t bytesReceived)
{
    if (!errCode) {
        handleRequestStatus();
    } else if (bytesReceived == 0UL) {
        _logs.logTo(logInfo.data(), "The received packet was empty. " + errCode.message());
    } else {
        _logs.logTo(logErr.data(), "Error receiving packet: " + errCode.message());
    }
    receive();
}

void net::Network::receive()
{
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

std::string net::Network::addClient()
{
    std::string cliUuid = uuid::generateUUID();
    _clients.push_back(Client(cliUuid, _serverEndpoint));
    return cliUuid;
}

void net::Network::removeClient(const std::string &uuid)
{
    std::vector<Client>::iterator it = std::remove_if(_clients.begin(), _clients.end(),
        [&](const Client &client) {
            return client.getUuid() == uuid.data();
        }
    );

    if (it != _clients.end()) {
        _clients.erase(it, _clients.end());
    }
}