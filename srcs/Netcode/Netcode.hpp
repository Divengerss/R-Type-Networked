#ifndef NETCODE_HPP
#define NETCODE_HPP

#include <string>
#include <cstdint>
#include <asio.hpp>
#include <iostream>
#include "CFGParser.hpp"
#include "Packet.hpp"

// Used to initialize the server, host and port are set from the CFG config file.
// If the parsing fails, these values will be used
#define DEFAULT_HOST    "127.0.0.1"
#define DEFAULT_PORT    12345

#define CONFIG_FILE_PATH   "/server.cfg"

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
                _socket(ioContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), DEFAULT_PORT))
            {
                utils::ParseCFG config(utils::getCurrDir() + CONFIG_FILE_PATH);
                try {
                    _host = config.getData<std::string>("host");
                    _port = std::stoi(config.getData<std::string>("port"));
                } catch (const Error &e) {
                    std::cerr << e.what() << std::endl;
                    setConnection(DEFAULT_HOST, DEFAULT_PORT);
                }
                asio::ip::udp::socket::reuse_address option(true);
                _socket.set_option(option);
                _socket = asio::ip::udp::socket(ioContext, asio::ip::udp::endpoint(asio::ip::make_address(_host), _port));
                _threadPool.emplace_back([&]()
                {
                    ioContext.run();
                });
                _threadPool.emplace_back([&]()
                {
                    ioService.run();
                });
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

            void handleReceiveFrom(const asio::error_code &err_code)
            {
                std::cout << "Entering handle receive" << std::endl;
                receive();
                std::cout << "Exiting handle receive" << std::endl;
            }

            void receive()
            {
                std::cout << "Entering receive" << std::endl;
                _socket.async_receive_from(
                    asio::buffer(_buffer),
                    _serverEndpoint,
                    std::bind(&Server::handleReceiveFrom, this, _errCode)
                );
                std::cout << "Exiting receive" << std::endl;
            }

            void startServer() {
                receive();
            }

        private:
            asio::io_context &_ioContext;
            asio::io_context &_ioService;
            std::string _host;
            std::uint16_t _port;
            asio::ip::udp::endpoint _serverEndpoint;
            asio::ip::udp::socket _socket;
            asio::error_code _errCode;
            std::vector<std::thread> _threadPool;
            std::array<char, 1024> _buffer;
    };

    class Client
    {

    };
};

#endif /* !NETCODE_HPP */