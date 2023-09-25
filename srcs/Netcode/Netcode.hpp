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

// Used to initialize the server, host and port are set from the CFG config file.
// If the parsing fails, these values will be used
#define DEFAULT_HOST    "127.0.0.1"
#define DEFAULT_PORT    12345

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

            ~Server() = default;

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
            void fillBufferFromRequest(T &packet, std::size_t size)
            {
                std::memcpy(&packet, _buffer.data(), size);
            }

            template<typename T>
            void sendResponse(T &packet)
            {
                _socket.send_to(asio::buffer(&packet, sizeof(packet)), _serverEndpoint);
            }

            template<typename T>
            void handleRequestStatus(T &packet)
            {
                if (packet.status == REQUEST && packet.type == packet::CONNECTION_REQUEST) {
                    std::string cliUuid = addClient();
                    packet::connectionRequest response(ACCEPTED, cliUuid);
                    _logs.logTo(INFO, "New connection received: UUID [" + cliUuid + "]");
                    sendResponse(response);
                } else if (packet.status == REQUEST && packet.type == packet::DISCONNECTION_REQUEST) {
                    std::string cliUuid(UUID_SIZE, 0);
                    std::memcpy(cliUuid.data(), &packet.uuid, UUID_SIZE);
                    _clients.erase(cliUuid);
                    _logs.logTo(INFO, "Disconnection received from [" + cliUuid + "]");
                    packet::disconnectionRequest response(ACCEPTED);
                    sendResponse(response);
                }
            }

            void handleReceiveFrom(const asio::error_code &errCode, std::size_t bytesReceived)
            {
                if (!errCode) {
                    packet::connectionRequest request;
                    fillBufferFromRequest(request, bytesReceived);
                    handleRequestStatus(request);
                } else {
                    _logs.logTo(ERR, "Error receiving packet: " + errCode.message());
                }
                receive();
            }

            void receive()
            {
                packet::connectionRequest request;
                _socket.async_receive_from(
                    asio::buffer(_buffer, _buffer.size()),
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
            std::array<std::uint8_t, 1024> _buffer;
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
                _socket(asio::ip::udp::socket(ioContext)), _uuid(UUID_SIZE, 0)
            {
                try {
                    _socket.open(asio::ip::udp::v4());
                } catch (const asio::system_error &sysErr) {
                    std::cerr << "Error opening socket: " << sysErr.what() << std::endl;
                    throw;
                }
                _network = std::thread([&] () {
                    std::signal(SIGINT, signalHandler);
                    setClientInstance(this);
                    connect();
                    listenServer();
                });
            }
            ~Client()
            {
                if (_network.joinable())
                    _network.join();
                _socket.close();
            };

            static void setClientInstance(Client* instance) {clientInstance = instance;}

            template<typename T>
            T &sendToServer(T &packet)
            {
                _socket.send_to(asio::buffer(&packet, sizeof(packet)), _endpoint);
                _socket.receive_from(asio::buffer(&packet, sizeof(packet)), _endpoint);
                return packet;
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
                packet::placeholder data;
                _socket.receive_from(asio::buffer(&data, sizeof(data)), _endpoint);
                handleReceive(data);
            }

            template<typename T>
            void handleReceive(T &packet)
            {
                if (!_errCode) {
                    if (packet.type == packet::PLACEHOLDER) {
                        std::cout << "Received a placeholder packet from server" << std::endl;
                    }
                    if (!_uuid.empty())
                        listenServer();
                }
            }

            void connect()
            {
                packet::connectionRequest request;
                request = sendToServer(request);
                if (request.type != packet::CONNECTION_REQUEST)
                    waitForPacket(request, packet::CONNECTION_REQUEST);
                std::memcpy(_uuid.data(), request.uuid.data(), UUID_SIZE);
                std::cout << "Got UUID " << _uuid << " from the server" << std::endl;
            }

            void disconnect()
            {
                packet::disconnectionRequest request(_uuid.data());
                request = sendToServer(request);
                std::cout << "Disconnected from the server." << std::endl;
                _uuid.clear();
            }

            static void signalHandler(int signum) {
                if (signum == SIGINT && clientInstance) {
                    clientInstance->disconnect();
                }
                std::exit(EXIT_SUCCESS);
            }

        private:
            asio::error_code _errCode;
            asio::ip::udp::resolver _resolver;
            asio::ip::udp::endpoint _endpoint;
            asio::ip::udp::socket _socket;
            std::string _uuid;
            std::thread _network;
            static Client* clientInstance;
    };

    Client* Client::clientInstance = nullptr;
};

#endif /* !NETCODE_HPP */