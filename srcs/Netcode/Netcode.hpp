#ifndef NETCODE_HPP
#define NETCODE_HPP

#include <string>
#include <cstdint>
#include <asio.hpp>
#include <iostream>
#include <csignal>
#include "CFGParser.hpp"
#include "Packets.hpp"
#include "Logs.hpp"
#include "Uuid.hpp"

// Default values used if parsing fails or invalid values are set.
static constexpr std::string_view defaultHost = "127.0.0.1";
static constexpr std::uint16_t defaultPort = 12345U;
static constexpr std::uint8_t defaultTimeout = 5U;
static constexpr std::uint32_t packetSize = 1024U;

#ifdef _WIN32
    static constexpr std::string_view serverConfigFilePath = "\\server.cfg";
    static constexpr std::string_view clientConfigFilePath = "\\client.cfg";
    static constexpr std::string_view serverLogFile = "\\server.log";
#else
    static constexpr std::string_view serverConfigFilePath = "/server.cfg";
    static constexpr std::string_view clientConfigFilePath = "/client.cfg";
    static constexpr std::string_view serverLogFile = "/server.log";
#endif /* !_WIN32 */

namespace net
{
    class Server
    {
        public:
            Server() = delete;
            Server(asio::io_context &ioContext, asio::io_context &ioService) :
                _ioContext(ioContext),
                _ioService(ioService),
                _host(defaultHost),
                _port(defaultPort),
                _socket(_ioContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), defaultPort)),
                _logs(Log(utils::getCurrDir() + serverLogFile.data())),
                _clients({})
            {
                _logs.logTo(logInfo.data(), "Starting up R-Type server...");
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
                for (auto &thread : _threadPool) {
                    if (thread.joinable()) {
                        thread.join();
                    }
                }
            };

            asio::io_context &getIoContext() const noexcept {return _ioContext;}
            asio::io_context &getIoService() const noexcept {return _ioService;}
            const std::string &getHost() const noexcept {return _host;}
            std::uint16_t getPort() const noexcept {return _port;}
            const asio::ip::udp::endpoint &getServerEndpoint() const noexcept {return _serverEndpoint;}
            const asio::ip::udp::socket &getSocket() const noexcept {return _socket;}
            const asio::error_code &getAsioErrorCode() const noexcept {return _errCode;}
            const std::unordered_map<std::string, asio::ip::udp::endpoint> &getClients() const noexcept {return _clients;}

            void setHost(const std::string &host) {_host = host;}
            void setPort(std::uint16_t port) {_port = port;}
            void setConnection(const std::string &host, std::uint16_t port) {_host = host; _port = port;}
            static void setServerInstance(Server* instance) {serverInstance = instance;}

            template<typename T>
            void setPacket(T &packet, packet::packetHeader &header, T&data)
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
            void sendResponse(const packet::packetTypes &type, T &packet, const std::string cliUuid = "")
            {
                packet::packetHeader header(type, sizeof(packet));
                std::array<std::uint8_t, sizeof(header) + sizeof(packet)> buffer;
                std::memmove(&buffer, &header, sizeof(header));
                std::memmove(&buffer[sizeof(header)], &packet, header.dataSize);
                if (cliUuid.empty() && !_clients.empty())
                    _logs.logTo(logInfo.data(), "Sending packet type [" + std::to_string(header.type) + "] to all clients:");
                for (const auto &[uuid, client] : _clients) {
                    if (!cliUuid.empty()) {
                        if (cliUuid == uuid) {
                            _socket.send_to(asio::buffer(&buffer, sizeof(buffer)), client);
                            _logs.logTo(logInfo.data(), "Sent packet type [" + std::to_string(header.type) + "] to [" + cliUuid + "]");
                        }
                    } else {
                        _socket.send_to(asio::buffer(&buffer, sizeof(buffer)), client);
                        _logs.logTo(logInfo.data(), "    Sent to [" + uuid + "]");
                    }
                }
            }

            void handleRequestStatus()
            {
                packet::packetHeader header = *reinterpret_cast<packet::packetHeader *>(_packet.data());
                if (header.type == packet::CONNECTION_REQUEST) {
                    std::string cliUuid = addClient();
                    packet::connectionRequest response(packet::ACCEPTED, cliUuid);
                    _logs.logTo(logInfo.data(), "New connection received: UUID [" + cliUuid + "]");
                    header.type = packet::CONNECTION_REQUEST;
                    header.dataSize = sizeof(response);
                    std::array<std::uint8_t, sizeof(header) + sizeof(response)> buffer;
                    std::memmove(&buffer, &header, sizeof(header));
                    std::memmove(&buffer[sizeof(header)], &response, sizeof(response));
                    _socket.send_to(asio::buffer(&buffer, sizeof(buffer)), _serverEndpoint);
                    packet::clientStatus cliStatus(cliUuid, packet::NEW_CLIENT);
                    sendResponse(packet::CLIENT_STATUS, cliStatus);
                } else if (header.type == packet::DISCONNECTION_REQUEST) {
                    packet::disconnectionRequest request(packet::ACCEPTED);
                    std::memmove(&request, &_packet[sizeof(header)], sizeof(request));
                    std::string cliUuid(uuidSize, 0);
                    std::memmove(cliUuid.data(), &request.uuid, uuidSize);
                    _clients.erase(cliUuid.data());
                    _logs.logTo(logInfo.data(), "Disconnection received from [" + cliUuid + "]");
                    packet::clientStatus cliStatus(cliUuid, packet::LOSE_CLIENT);
                    sendResponse(packet::CLIENT_STATUS, cliStatus);
                }
            }

            void handleReceiveFrom(const asio::error_code &errCode, std::size_t bytesReceived)
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
                    std::bind(&Server::handleReceiveFrom, this, std::placeholders::_1, std::placeholders::_2)
                );
            }

            void startServer() {
                setServerInstance(this);
                _logs.logTo(logInfo.data(), "Server initialized successfully");
                _logs.logTo(logInfo.data(), "Listening at " + _host + " on port " + std::to_string(_port));
                receive();
            }

            static void signalHandler(int signum) {
                if (signum == SIGINT && serverInstance) {
                    serverInstance->_logs.logTo(logInfo.data(), "Received SIGINT (CTRL-C). Shutting down gracefully...");
                    serverInstance->_ioContext.stop();
                    serverInstance->_ioService.stop();
                    for (auto &thread : serverInstance->_threadPool) {
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
                _clients[cliUuid] = _serverEndpoint;
                return cliUuid;
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
            std::unordered_map<std::string, asio::ip::udp::endpoint> _clients;
            static Server* serverInstance;

    };

    Server* Server::serverInstance = nullptr;

    class Client
    {
        public:
            Client() = delete;
            Client(asio::io_context &ioContext, const std::string &host, const std::string &port) :
                _errCode(asio::error_code()), _resolver(ioContext), _endpoint(*_resolver.resolve(asio::ip::udp::v4(), host, port).begin()),
                _socket(asio::ip::udp::socket(ioContext)), _timer(ioContext), _uuid(uuidSize, 0), _packet({})
            {
                try {
                    utils::ParseCFG config(utils::getCurrDir() + clientConfigFilePath.data());
                    std::uint8_t timeout;
                    try {
                        timeout = static_cast<std::uint8_t>(std::stoi(config.getData<std::string>("timeout")));
                        if (timeout < 1U) {
                            std::cout << "Warning: Timeout value should not be lower than 1 second. " << defaultTimeout << " seconds will be used instead." << std::endl;
                            timeout = defaultTimeout;
                        }
                    } catch (const Error &e) {
                        std::cerr << e.what() << std::endl;
                        timeout = defaultTimeout;
                    }
                    _timer.expires_after(std::chrono::seconds(timeout));
                    _socket.open(asio::ip::udp::v4());
                } catch (const asio::system_error &sysErr) {
                    std::cerr << "Error opening socket: " << sysErr.what() << std::endl;
                    throw;
                }
                _threadPool.emplace_back([&] () {
                    std::signal(SIGINT, signalHandler);
                    setClientInstance(this);
                    _timer.async_wait([&](const asio::error_code &error) {
                        if (!error) {
                            _socket.cancel();
                            if (!std::strncmp(_uuid.data(), std::string(uuidSize, 0).data(), uuidSize)) {
                                std::cerr << "Connection timeout. Could not connect to server." << std::endl;
                                std::exit(EXIT_SUCCESS);
                            }
                        }
                    });
                    listenServer();
                    connect();
                    ioContext.run();
                });
            }
            ~Client()
            {
                for (auto &thread : clientInstance->_threadPool) {
                    if (thread.joinable()) {
                        thread.join();
                    }
                }
                _socket.close();
            };

            static void setClientInstance(Client* instance) {clientInstance = instance;}

            template<typename T>
            void sendToServer(T &packet)
            {
                _socket.send_to(asio::buffer(&packet, sizeof(packet)), _endpoint);
            }

            template<typename T>
            T &waitForPacket(T &packet, std::uint8_t type)
            {
                while (packet.type != type) {
                    _socket.receive_from(asio::buffer(&packet, sizeof(packet)), _endpoint);
                    std::cout << "Got type " << std::to_string(packet.type) << std::endl;
                }
                return packet;
            }

            void listenServer()
            {
                try {
                    _socket.async_receive_from(
                        asio::buffer(&_packet, sizeof(_packet)),
                        _endpoint,
                        std::bind(&Client::handleReceive, this, std::placeholders::_1));
                } catch (const asio::system_error &e) {
                    std::cerr << "Failed to receive packet: " << e.what() << std::endl;
                }
            }

            void handleReceive(const asio::error_code &errCode)
            {
                if (!errCode) {
                    packet::packetHeader header;
                    std::memmove(&header, _packet.data(), sizeof(header));
                    if (header.type == packet::PLACEHOLDER) {
                        std::cout << "Received a placeholder packet from server" << std::endl;
                    } else if (header.type == packet::CONNECTION_REQUEST) {
                        packet::connectionRequest request;
                        std::memmove(&request, &_packet[sizeof(header)], header.dataSize);
                        std::memmove(_uuid.data(), &request.uuid, uuidSize);
                        std::cout << "Got uuid = " << _uuid << std::endl;
                    } else if (header.type == packet::CLIENT_STATUS) {
                        packet::clientStatus cliStatus;
                        std::memmove(&cliStatus, &_packet[sizeof(header)], sizeof(cliStatus));
                        std::string cliUuid(reinterpret_cast<char *>(cliStatus.uuid.data()));
                        if (cliStatus.status == packet::LOSE_CLIENT && std::strcmp(cliUuid.c_str(), _uuid.c_str())) {
                            std::cout << "Client " << cliUuid << " disconnected." << std::endl;
                        } else if (cliStatus.status == packet::NEW_CLIENT && std::strcmp(cliUuid.c_str(), _uuid.c_str())) {
                            std::cout << "Client " << cliUuid << " connected." << std::endl;
                        }
                    }
                }
                _timer.cancel();
                listenServer();
            }

            void connect()
            {
                packet::connectionRequest request;
                packet::packetHeader header(packet::CONNECTION_REQUEST, sizeof(request));
                std::array<std::uint8_t, sizeof(header) + sizeof(request)> buffer;
                std::memmove(&buffer, &header, sizeof(header));
                std::memmove(&buffer[sizeof(header)], &request, sizeof(request));
                sendToServer(buffer);
            }

            void disconnect()
            {
                packet::disconnectionRequest request(_uuid.data());
                packet::packetHeader header(packet::DISCONNECTION_REQUEST, sizeof(request));
                std::array<std::uint8_t, sizeof(header) + sizeof(request)> buffer;
                std::memmove(&buffer, &header, sizeof(header));
                std::memmove(&buffer[sizeof(header)], &request, sizeof(request));
                sendToServer(buffer);
                std::cout << "Disconnected from the server." << std::endl;
                _uuid.clear();
            }

            static void signalHandler(int signum) {
                if (signum == SIGINT && clientInstance) {
                    if (!clientInstance->_uuid.empty())
                        clientInstance->disconnect();
                }
                std::exit(EXIT_SUCCESS);
            }

        private:
            asio::error_code _errCode;
            asio::ip::udp::resolver _resolver;
            asio::ip::udp::endpoint _endpoint;
            asio::ip::udp::socket _socket;
            asio::steady_timer _timer;
            std::string _uuid;
            std::vector<std::thread> _threadPool;
            std::array<std::uint8_t, packetSize> _packet;
            static Client* clientInstance;
    };

    Client* Client::clientInstance = nullptr;
};

#endif /* !NETCODE_HPP */