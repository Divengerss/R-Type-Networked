#ifndef NETCODE_HPP
#define NETCODE_HPP

#include <string>
#include <cstdint>
#include <asio.hpp>
#include <iostream>
#include <csignal>
#include "CFGParser.hpp"
#include "Packet.hpp"
#include "Logs.hpp"

// Used to initialize the server, host and port are set from the CFG config file.
// If the parsing fails, these values will be used
#define DEFAULT_HOST    "127.0.0.1"
#define DEFAULT_PORT    12345

#define CONFIG_FILE_PATH   "/server.cfg"
#define SERVER_LOG_FILE    "/server.log"

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
                _logs(Log(utils::getCurrDir() + SERVER_LOG_FILE))
            {
                _logs.logTo(INFO, "Starting up R-Type server...");
                utils::ParseCFG config(utils::getCurrDir() + CONFIG_FILE_PATH);
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
                _socket = asio::ip::udp::socket(_ioContext, asio::ip::udp::endpoint(asio::ip::make_address(_host), _port));
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

            void setHost(const std::string &host) {_host = host;}
            void setPort(std::uint16_t port) {_port = port;}
            void setConnection(const std::string &host, std::uint16_t port) {_host = host; _port = port;}
            static void setServerInstance(Server* instance) {serverInstance = instance;}

            void handleReceiveFrom(const asio::error_code &err_code)
            {
                _logs.logTo(INFO, "TEST PURPOSE, Received connection!");
                receive();
            }

            void receive()
            {
                _socket.async_receive_from(
                    asio::buffer(_buffer),
                    _serverEndpoint,
                    std::bind(&Server::handleReceiveFrom, this, _errCode)
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

        private:
            asio::io_context &_ioContext;
            asio::io_service &_ioService;
            std::string _host;
            std::uint16_t _port;
            asio::ip::udp::endpoint _serverEndpoint;
            asio::ip::udp::socket _socket;
            asio::error_code _errCode;
            std::vector<std::thread> _threadPool;
            std::array<char, 1024> _buffer;
            Log _logs;
            static Server* serverInstance;
    };

    Server* Server::serverInstance = nullptr;

    class Client
    {
    };
};

#endif /* !NETCODE_HPP */