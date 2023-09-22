#ifndef NETCODE_HPP
#define NETCODE_HPP

#include <string>
#include <cstdint>
#include <asio.hpp>
#include <iostream>
#include "CFGParser.hpp"

// Used to initialize the server, host and port are set from the CFG config file.
// If the parsing fails, these values will be used
#define DEFAULT_HOST    "127.0.0.1"
#define DEFAULT_PORT    60000

#define CONFIG_FILE_PATH   "/server.cfg"

namespace net
{
    class Server
    {
        public:
            Server() = delete;
            Server(asio::io_context &ioContext, asio::io_context &ioService) :
                _ioContext(ioContext), _ioService(ioService), _host(DEFAULT_HOST), _port(DEFAULT_PORT),
                _socket(asio::ip::udp::socket(ioContext, asio::ip::udp::endpoint(asio::ip::make_address(DEFAULT_HOST), DEFAULT_PORT)))
            {
                utils::ParseCFG config(utils::getCurrDir() + CONFIG_FILE_PATH);
                try {
                    _host = config.getData<std::string>("host");
                    _port = std::stoi(config.getData<std::string>("port"));
                } catch (const Error &e) {
                    std::cerr << e.what() << std::endl;
                    _host = DEFAULT_HOST;
                    _port = DEFAULT_PORT;
                }
                std::cout << _host << " " << _port << std::endl;
                asio::ip::udp::socket::reuse_address option(true);
                _socket.set_option(option);
                _socket = asio::ip::udp::socket(ioContext, asio::ip::udp::endpoint(asio::ip::make_address(_host), _port));
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

        private:
            asio::io_context &_ioContext;
            asio::io_context &_ioService;
            std::string _host;
            std::uint16_t _port;
            asio::ip::udp::endpoint _serverEndpoint;
            asio::ip::udp::socket _socket;
            asio::error_code _errCode;
    };

    class Client
    {

    };
};

#endif /* !NETCODE_HPP */