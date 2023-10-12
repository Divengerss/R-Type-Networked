#ifndef NETWORK_HPP_
#define NETWORK_HPP_

#include <string>
#include <cstdint>
#include <asio.hpp>
#include <iostream>
#include <csignal>
#include "CFGParser.hpp"
#include "Packets.hpp"
#include <functional>

// Default values used if parsing fails or invalid values are set.
static constexpr std::string_view defaultHost = "127.0.0.1";
static constexpr std::uint16_t defaultPort = 12345U;
static constexpr std::uint8_t defaultTimeout = 5U;
static constexpr std::uint32_t localPacketSize = 1024U;

#ifdef _WIN32
    static constexpr std::string_view clientConfigFilePath = "\\client.cfg";
#else
    static constexpr std::string_view clientConfigFilePath = "/client.cfg";
#endif /* !_WIN32 */

namespace net
{
    class Client
    {
        public:
            Client() = delete;
            Client(asio::io_context &ioContext, const std::string &host, const std::string &port) :
                _ioContext(ioContext), _errCode(asio::error_code()), _resolver(ioContext), _endpoint(*_resolver.resolve(asio::ip::udp::v4(), host, port).begin()),
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
                    _ioContext.run();
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

            bool isSocketOpen() const noexcept
            {
                return _socket.is_open();
            }

            static void setClientInstance(Client* instance) {clientInstance = instance;}

            template<typename T>
            std::size_t sendPacket(packet::packetHeader &header, T &data)
            {
                const std::size_t headerSize = sizeof(header);
                const std::size_t dataSize = sizeof(data);
                std::array<std::uint8_t, headerSize + dataSize> packet;

                std::memmove(&packet, &header, headerSize);
                std::memmove(&packet[headerSize], &data, dataSize);
                return _socket.send_to(asio::buffer(&packet, headerSize + dataSize), _endpoint);
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
                } catch (const Error &) {
                    std::cerr << "Server connection closed" << std::endl;
                }
            }

            void handlePlaceholderPacket() {
                std::cout << "Received a placeholder packet from server" << std::endl;
            }

            void handleConnectionRequestPacket(const packet::connectionRequest &request) {
                std::memmove(_uuid.data(), &request.uuid, uuidSize);
                std::cout << "Got uuid = " << _uuid << std::endl;
            }

            void handleClientStatusPacket(packet::clientStatus &cliStatus) {
                std::string cliUuid(reinterpret_cast<char *>(cliStatus.uuid.data()));
                if (cliStatus.status == packet::LOSE_CLIENT && std::strcmp(cliUuid.c_str(), _uuid.c_str())) {
                    std::cout << "Client " << cliUuid << " disconnected." << std::endl;
                } else if (cliStatus.status == packet::NEW_CLIENT && std::strcmp(cliUuid.c_str(), _uuid.c_str())) {
                    std::cout << "Client " << cliUuid << " connected." << std::endl;
                }
            }

            void handleForceDisconnectPacket() {
                _ioContext.stop();
                _socket.close();
                std::cout << "Disconnection received from server." << std::endl;
            }

            void handleReceive(const asio::error_code &errCode) {
                packet::packetHeader header;

                if (!errCode) {
                    std::memmove(&header, _packet.data(), sizeof(header));
                    std::unordered_map<std::uint16_t, std::function<void()>> packetHandlers = {
                        {packet::PLACEHOLDER, [&]{ handlePlaceholderPacket(); }},
                        {packet::CONNECTION_REQUEST, [&]{
                            packet::connectionRequest request;
                            std::memmove(&request, &_packet[sizeof(header)], header.dataSize);
                            handleConnectionRequestPacket(request);
                        }},
                        {packet::CLIENT_STATUS, [&]{
                            packet::clientStatus cliStatus;
                            std::memmove(&cliStatus, &_packet[sizeof(header)], sizeof(cliStatus));
                            handleClientStatusPacket(cliStatus);
                        }},
                        {packet::FORCE_DISCONNECT, [&]{ handleForceDisconnectPacket(); }},
                    };

                    auto handlerIt = packetHandlers.find(header.type);
                    if (handlerIt != packetHandlers.end()) {
                        handlerIt->second();
                    } else {
                        std::cerr << "Unknown packet" << std::endl;
                    }
                }

                _timer.cancel();
                if (!errCode && header.type != packet::FORCE_DISCONNECT) {
                    listenServer();
                } else {
                    listenServer();
                }
            }

            void connect()
            {
                packet::connectionRequest request;
                packet::packetHeader header(packet::CONNECTION_REQUEST, sizeof(request));
                std::size_t bytesSent = sendPacket(header, request);
                if (bytesSent == 0UL)
                    std::cerr << "Something went wrong sending the packet connection." << std::endl;
            }

            void disconnect()
            {
                packet::disconnectionRequest request(_uuid.data());
                packet::packetHeader header(packet::DISCONNECTION_REQUEST, sizeof(request));
                std::size_t bytesSent = sendPacket(header, request);
                if (bytesSent == 0UL)
                    std::cerr << "Something went wrong sending the packet disconnection." << std::endl;
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
            asio::io_context &_ioContext;
            asio::error_code _errCode;
            asio::ip::udp::resolver _resolver;
            asio::ip::udp::endpoint _endpoint;
            asio::ip::udp::socket _socket;
            asio::steady_timer _timer;
            std::string _uuid;
            std::vector<std::thread> _threadPool;
            std::array<std::uint8_t, localPacketSize> _packet;
            static Client* clientInstance;
    };

    Client* Client::clientInstance = nullptr;
}; // namespace net

#endif /* !NETWORK_HPP_ */