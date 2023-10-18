#ifndef NETWORK_HPP_
#define NETWORK_HPP_

#include <string>
#include <cstdint>
#include <asio.hpp>
#include <iostream>
#include <csignal>
#include "CFGParser.hpp"
#include "Packets.hpp"
#include "Logs.hpp"
#include "Uuid.hpp"
#include "SparseArray.hpp"
#include "Registry.hpp"
#include "Position.hpp"
#include "Velocity.hpp"
#include "Controllable.hpp"
#include "Destroyable.hpp"
#include "MovementPattern.hpp"
#include "Hitbox.hpp"

// Default values used if parsing fails or invalid values are set.
static constexpr std::string_view defaultHost = "127.0.0.1";
static constexpr std::uint16_t defaultPort = 12345U;
static constexpr std::uint32_t packetSize = 1024U;

#ifdef _WIN32
    static constexpr std::string_view serverConfigFilePath = "\\server.cfg";
    static constexpr std::string_view serverLogFile = "\\server.log";
#else
    static constexpr std::string_view serverConfigFilePath = "/server.cfg";
    static constexpr std::string_view serverLogFile = "/server.log";
#endif /* !_WIN32 */

namespace net
{
    class Client
    {
        public:
            Client() = delete;
            Client(const std::string &uuid, asio::ip::udp::endpoint &endpoint) : _uuid(uuid), _endpoint(endpoint) {}
            ~Client() = default;

            const std::string &getUuid() const noexcept { return _uuid; };
            asio::ip::udp::endpoint &getEndpoint() noexcept { return _endpoint; };

        private:
            std::string _uuid;
            asio::ip::udp::endpoint _endpoint;
    };

    class Server
    {
        public:
            Server() = delete;
            Server(asio::io_context &ioContext, asio::io_service &ioService, Registry &reg) :
                _ioContext(ioContext), _ioService(ioService),
                _host(defaultHost), _port(defaultPort),
                _socket(_ioContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), defaultPort)),
                _logs(Log(utils::getCurrDir() + serverLogFile.data())),
                _clients({}), _reg(reg)
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
                asio::ip::udp::socket::reuse_address option(true);
                _socket.set_option(option);
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
            };

            ~Server()
            {
                for (std::thread &thread : _threadPool) {
                    if (thread.joinable()) {
                        thread.join();
                    }
                }
                _logs.logTo(logInfo.data(), "Server has been shut down successfully.");
            };

            asio::io_context &getIoContext() const noexcept {return _ioContext;}
            asio::io_context &getIoService() const noexcept {return _ioService;}
            const std::string &getHost() const noexcept {return _host;}
            std::uint16_t getPort() const noexcept {return _port;}
            const asio::ip::udp::endpoint &getServerEndpoint() const noexcept {return _serverEndpoint;}
            const asio::ip::udp::socket &getSocket() const noexcept {return _socket;}
            const asio::error_code &getAsioErrorCode() const noexcept {return _errCode;}
            const std::vector<Client> &getClients() const noexcept {return _clients;}

            void setHost(const std::string &host) {_host = host;}
            void setPort(std::uint16_t port) {_port = port;}
            void setConnection(const std::string &host, std::uint16_t port) {_host = host; _port = port;}
            static void setServerInstance(Server* instance) {serverInstance = instance;}

            bool isSocketOpen() const noexcept { return _socket.is_open(); }

            template<typename T>
            void setPacket(T &packet, packet::packetHeader &header, T &data)
            {
                std::memmove(&packet, &header, sizeof(header));
                std::memmove(&packet[sizeof(header)], &data, header.dataSize);
            }

            template<typename T>
            void fillBufferFromRequest(T &packet, std::size_t size)
            {
                std::memmove(&packet, _packet.data(), size);
            }

            template<typename T>
            void sendResponse(const packet::packetTypes &type, T &packet, const std::string cliUuid = "", bool toServerEndpoint = false)
            {
                packet::packetHeader header(type, sizeof(packet));
                std::array<std::uint8_t, sizeof(header) + sizeof(packet)> buffer;
                std::memmove(&buffer, &header, sizeof(header));
                std::memmove(&buffer[sizeof(header)], &packet, header.dataSize);
                if (toServerEndpoint)
                    _socket.send_to(asio::buffer(&buffer, sizeof(buffer)), _serverEndpoint);
                else if (cliUuid.empty() && !_clients.empty())
                    _logs.logTo(logInfo.data(), "Sending packet type [" + std::to_string(header.type) + "] to all clients:");
                for (Client &client : _clients) {
                    if (!cliUuid.empty()) {
                        if (cliUuid == client.getUuid()) {
                            _socket.send_to(asio::buffer(&buffer, sizeof(buffer)), client.getEndpoint());
                            _logs.logTo(logInfo.data(), "Sent packet type [" + std::to_string(header.type) + "] to [" + cliUuid + "]");
                        }
                    } else {
                        _socket.send_to(asio::buffer(&buffer, sizeof(buffer)), client.getEndpoint());
                        _logs.logTo(logInfo.data(), "    Sent to [" + client.getUuid() + "]");
                    }
                }
            }

            template<class T>
            void sendSparseArray(const packet::packetTypes &type, sparse_array<T> &sparseArray, const std::string cliUuid = "")
            {
                const std::size_t componentSize = sizeof(T);
                const std::size_t componentsSize = (sizeof(bool) + componentSize) * sparseArray.size();
                packet::packetHeader header(type, componentsSize);
                const std::size_t headerSize = sizeof(header);
                // const std::size_t bufferSize = headerSize + componentsSize;
                // std::array<std::uint8_t, bufferSize> buffer;
                std::array<std::uint8_t, packetSize> buffer;
                std::size_t offset = 0UL;

                std::memmove(&buffer, &header, headerSize);
                for (auto &component : sparseArray) {
                    if (component.has_value()) {
                        bool isNullOpt = false;
                        std::memmove(&buffer[headerSize + offset], &isNullOpt, sizeof(bool));
                        offset += sizeof(bool);
                        std::memmove(&buffer[headerSize + offset], &component.value(), componentSize);
                    } else {
                        bool isNullOpt = true;
                        std::memmove(&buffer[headerSize + offset], &isNullOpt, sizeof(bool));
                        offset += sizeof(bool);
                    }
                    offset += componentSize;
                }
                if (cliUuid.empty() && !_clients.empty())
                    _logs.logTo(logInfo.data(), "Sending packet type [" + std::to_string(header.type) + "] to all clients:");
                for (Client &client : _clients) {
                    if (!cliUuid.empty()) {
                        if (cliUuid == client.getUuid()) {
                            _socket.send_to(asio::buffer(&buffer, sizeof(buffer)), client.getEndpoint());
                            _logs.logTo(logInfo.data(), "Sent packet type [" + std::to_string(header.type) + "] to [" + cliUuid + "]");
                        }
                    } else {
                        _socket.send_to(asio::buffer(&buffer, sizeof(buffer)), client.getEndpoint());
                        _logs.logTo(logInfo.data(), "    Sent to [" + client.getUuid() + "]");
                    }
                }
            }

            void handleConnectionRequest() {
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
                float posX = 30.0f;
                float posY = 250.0f * _clients.size();
                Position position(posX, posY);
                Controllable ctrl(cliUuid);
                Hitbox hb(99, 51);
                Entity entity = _reg.spawn_entity();
                _reg.add_component<Position>(entity, position);
                _reg.add_component<Velocity>(entity, 3);
                _reg.add_component<Controllable>(entity, ctrl);
                _reg.add_component<Hitbox>(entity, hb);
                _reg.add_component<Destroyable>(entity, 1);

                packet::clientStatus cliStatus(cliUuid, packet::NEW_CLIENT, posX, posY, _clients.size());
                try {
                    sendResponse(packet::CLIENT_STATUS, cliStatus);
                } catch (const std::system_error &e) {
                    std::string err = e.what();
                    _logs.logTo(logWarn.data(), "Failed to send the response of packet type [" + std::to_string(header.type) + "]:");
                    _logs.logTo(logWarn.data(), "    " + err);
                }
            }

            void handleDisconnectionRequest(packet::packetHeader &header) {
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
                for (auto &component : _reg.get_components<Controllable>()) {
                    std::string playId(uuidSize, 0);
                    std::memmove(playId.data(), &component->_playerId, uuidSize);
                    if (component.has_value() && !std::strcmp(playId.c_str(), cliUuid.c_str())) {
                        _reg.kill_entity(Entity(_reg.get_components<Controllable>().get_index(component)));
                    }
                }
            }

            void handleKeyboardEvent(packet::packetHeader& header) {
                packet::keyboardEvent event;
                std::memmove(&event, &_packet[sizeof(header)], sizeof(event));
                std::string uuid (uuidSize, 0);
                std::memmove(uuid.data(), &event.uuid, uuidSize);
                auto &conts = _reg.get_components<Controllable>();
                for (auto &cont : conts) {
                    std::string playId(uuidSize, 0);
                    std::memmove(playId.data(), &cont->_playerId, uuidSize);
                    if (cont && !std::strcmp(playId.c_str(), uuid.c_str())) {
                        cont->latestInput = event.keyCode;
                    }
                }
            }
            void handleRequestStatus() {
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

            void receiveCallback(const asio::error_code &errCode, std::size_t bytesReceived)
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

            void receive()
            {
                _socket.async_receive_from(
                    asio::buffer(_packet, _packet.size()),
                    _serverEndpoint,
                    std::bind(&Server::receiveCallback, this, std::placeholders::_1, std::placeholders::_2)
                );
            }

            void startServer()
            {
                setServerInstance(this);
                _logs.logTo(logInfo.data(), "Server initialized successfully");
                _logs.logTo(logInfo.data(), "Listening at " + _host + " on port " + std::to_string(_port));
                receive();
            }

            void stopServer()
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

            static void signalHandler(int signum) {
                if (signum == SIGINT && serverInstance) {
                    serverInstance->_logs.logTo(logInfo.data(), "Received SIGINT (CTRL-C). Shutting down gracefully...");
                    packet::packetHeader header(packet::FORCE_DISCONNECT, 0);
                    std::array<std::uint8_t, sizeof(header)> packet;
                    serverInstance->sendResponse(packet::FORCE_DISCONNECT, packet);
                    serverInstance->_ioContext.stop();
                    serverInstance->_ioService.stop();
                    for (std::thread &thread : serverInstance->_threadPool) {
                        if (thread.joinable()) {
                            thread.join();
                        }
                    }
                }
                std::exit(EXIT_SUCCESS);
            }

            std::string addClient()
            {
                std::string cliUuid = uuid::generateUUID();
                _clients.push_back(Client(cliUuid, _serverEndpoint));
                return cliUuid;
            }

            void removeClient(const std::string &uuid)
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

        private:
            asio::io_context &_ioContext;
            asio::io_service &_ioService;
            std::string _host;
            std::uint16_t _port;
            asio::ip::udp::endpoint _serverEndpoint;
            asio::ip::udp::socket _socket;
            asio::error_code _errCode;
            std::vector<std::thread> _threadPool;
            std::array<std::uint8_t, packetSize> _packet;
            Log _logs;
            std::vector<Client> _clients;
            Registry &_reg;
            static Server* serverInstance;
    };

    Server* Server::serverInstance = nullptr;
}

#endif /* !NETWORK_HPP_ */