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
#define DEFAULT_HOST        "127.0.0.1"
#define DEFAULT_PORT        12345
#define DEFAULT_TIMEOUT     5

#define PACKET_SIZE     1024

#ifdef _WIN32
    #define SERVER_CONFIG_FILE_PATH     "\\server.cfg"
    #define CLIENT_CONFIG_FILE_PATH     "\\client.cfg"
    #define SERVER_LOG_FILE             "\\server.log"
#else
    #define SERVER_CONFIG_FILE_PATH     "/server.cfg"
    #define CLIENT_CONFIG_FILE_PATH     "/client.cfg"
    #define SERVER_LOG_FILE             "/server.log"
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
                _host(DEFAULT_HOST),
                _port(DEFAULT_PORT),
                _socket(_ioContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), DEFAULT_PORT)),
                _logs(Log(utils::getCurrDir() + SERVER_LOG_FILE)),
                _clients({})
            {
                _logs.logTo(INFO, "Starting up R-Type server...");
                utils::ParseCFG config(utils::getCurrDir() + SERVER_CONFIG_FILE_PATH);
                try {
                    _host = config.getData<std::string>("host");
                    _port = std::stoi(config.getData<std::string>("port"));
                    _logs.logTo(INFO, "Retrieved configs successfully");
                } catch (const Error &e) {
                    std::string err = e.what();
                    setConnection(DEFAULT_HOST, DEFAULT_PORT);
                    _logs.logTo(ERR, "Failed to retrieve configs: " + err);
                    _logs.logTo(WARN, "Default host and ports applied");
                }
                asio::ip::udp::socket::reuse_address option(true);
                _socket.set_option(option);
                try {
                    _socket = asio::ip::udp::socket(_ioContext, asio::ip::udp::endpoint(asio::ip::make_address(_host), _port));
                } catch (const std::exception &e) {
                    std::string err = e.what();
                    _logs.logTo(ERR, "Failed to start the server: " + err);
                    throw std::runtime_error("bind");
                }
                _threadPool.emplace_back([&]() {
                    _logs.logTo(INFO, "Starting network execution context");
                    _ioContext.run();
                });
                _threadPool.emplace_back([&]() {
                    _logs.logTo(INFO, "Starting network execution service");
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
                std::memcpy(&packet, &header, sizeof(header));
                std::memcpy(&packet[sizeof(header)], &data, header.dataSize);
            }

            template<typename T>
            void fillBufferFromRequest(T &packet, std::size_t size)
            {
                std::memcpy(&packet, _packet.data(), size);
            }

            template<typename T>
            void sendResponse(const packet::packetTypes &type, T &packet)
            {
                packet::packetHeader header(type, sizeof(packet));
                std::array<std::uint8_t, sizeof(header) + sizeof(packet)> buffer;
                setPacket(buffer, header, packet);
                _socket.send_to(asio::buffer(&buffer, sizeof(buffer)), _serverEndpoint);
            }

            void handleRequestStatus()
            {
                packet::packetHeader header = *reinterpret_cast<packet::packetHeader *>(_packet.data());
                if (header.type == packet::CONNECTION_REQUEST) {
                    std::string cliUuid = addClient();
                    packet::connectionRequest response(ACCEPTED, cliUuid);
                    _logs.logTo(INFO, "New connection received: UUID [" + cliUuid + "]");
                    packet::packetHeader header(packet::CONNECTION_REQUEST, sizeof(response));
                    std::array<std::uint8_t, sizeof(header) + sizeof(response)> buffer;
                    std::memcpy(&buffer, &header, sizeof(header));
                    std::memcpy(&buffer[sizeof(header)], &response, sizeof(response));
                    _socket.send_to(asio::buffer(&buffer, sizeof(buffer)), _serverEndpoint);
                    packet::packetHeader header2;
                    packet::connectionRequest req;
                    std::memmove(&header2, &buffer, sizeof(header));
                    std::memmove(&req, &buffer[sizeof(header)], sizeof(req));
                } else if (header.type == packet::DISCONNECTION_REQUEST) {
                    packet::disconnectionRequest request(ACCEPTED);
                    std::memcpy(&request, &_packet[sizeof(header)], sizeof(request));
                    std::string cliUuid(UUID_SIZE, 0);
                    std::memcpy(cliUuid.data(), &request.uuid, UUID_SIZE);
                    _clients.erase(cliUuid.data());
                    _logs.logTo(INFO, "Disconnection received from [" + cliUuid + "]");
                    packet::disconnectionRequest response(ACCEPTED);
                }
            }

            void handleReceiveFrom(const asio::error_code &errCode, std::size_t bytesReceived)
            {
                if (!errCode) {
                    handleRequestStatus();
                } else if (bytesReceived == 0) {
                    _logs.logTo(INFO, "The received packet was empty. " + errCode.message());
                } else {
                    _logs.logTo(ERR, "Error receiving packet: " + errCode.message());
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
                _logs.logTo(INFO, "Server initialized successfully");
                _logs.logTo(INFO, "Listening at " + _host + " on port " + std::to_string(_port));
                receive();
            }

            static void signalHandler(int signum) {
                if (signum == SIGINT && serverInstance) {
                    serverInstance->_logs.logTo(INFO, "Received SIGINT (CTRL-C). Shutting down gracefully...");
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
            std::array<std::uint8_t, PACKET_SIZE> _packet;
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
                _socket(asio::ip::udp::socket(ioContext)), _timer(ioContext), _uuid(UUID_SIZE, 0)
            {
                try {
                    utils::ParseCFG config(utils::getCurrDir() + CLIENT_CONFIG_FILE_PATH);
                    std::uint8_t timeout;
                    try {
                        timeout = std::stoi(config.getData<std::string>("timeout"));
                        if (timeout < 1) {
                            std::cout << "Warning: Timeout value should not be lower than 1 second. " << DEFAULT_TIMEOUT << " seconds will be used instead." << std::endl;
                            timeout = DEFAULT_TIMEOUT;
                        }
                    } catch (const Error &e) {
                        std::cerr << e.what() << std::endl;
                        timeout = DEFAULT_TIMEOUT;
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
                            if (!std::strncmp(_uuid.data(), std::string(UUID_SIZE, 0).data(), UUID_SIZE)) {
                                std::cerr << "Connection timeout. Could not connect to server." << std::endl;
                                std::exit(0);
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
                    std::memcpy(&header, &_packet, sizeof(header));
                    if (header.type == packet::PLACEHOLDER) {
                        std::cout << "Received a placeholder packet from server" << std::endl;
                    } else if (header.type == packet::CONNECTION_REQUEST) {
                        packet::connectionRequest request;
                        std::memcpy(&request, &_packet[sizeof(header)], header.dataSize);
                        std::memcpy(_uuid.data(), &request.uuid, UUID_SIZE);
                        std::cout << "Got uuid = " << _uuid << std::endl;
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
                std::memcpy(&buffer, &header, sizeof(header));
                std::memcpy(&buffer[sizeof(header)], &request, sizeof(request));
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
            std::array<std::uint8_t, PACKET_SIZE> _packet;
            static Client* clientInstance;
    };

    Client* Client::clientInstance = nullptr;
};

#endif /* !NETCODE_HPP */