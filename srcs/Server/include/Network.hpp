/*
** EPITECH PROJECT, 2023
** R-Type
** File description:
** Network
*/

#ifndef NETWORK_HPP_
#define NETWORK_HPP_

#include <string>
#include <cstdint>
#include <asio.hpp>
#include <iostream>
#include <csignal>
#include "ZLib.hpp"

#include "Packets.hpp"
#include "Registry.hpp"
#include "Logs.hpp"
// #include "Engine.hpp"

// Default values used if parsing fails or invalid values are set.
static constexpr std::string_view defaultHost = "127.0.0.1";
static constexpr std::uint16_t defaultPort = 12345U;
static constexpr std::uint32_t packetSize = 1024U;

#ifdef _WIN32
    static constexpr std::string_view serverConfigFilePath = "\\resources\\server.cfg";
    static constexpr std::string_view serverLogFile = "\\server.log";
#else
    static constexpr std::string_view serverConfigFilePath = "/resources/server.cfg";
    static constexpr std::string_view serverLogFile = "/server.log";
#endif /* !_WIN32 */

namespace net
{
    class Client
    {
        public:
            Client() = delete;
            Client(const std::string &uuid, asio::ip::udp::endpoint &endpoint) : _uuid(uuid), _endpoint(endpoint), _missedPacket(0UL) {}
            Client(const std::string &uuid, asio::ip::udp::endpoint &endpoint, std::uint64_t roomId) : _uuid(uuid), _endpoint(endpoint), _missedPacket(0UL), _roomId(roomId) {}
            ~Client() = default;

            const std::string &getUuid() const noexcept { return _uuid; };
            asio::ip::udp::endpoint &getEndpoint() noexcept { return _endpoint; };
            std::size_t getMissedPacket() const noexcept { return _missedPacket; };
            std::uint64_t getRoomId() const noexcept { return _roomId; };

            void packetMissed() { _missedPacket += 1UL; };
            void resetMissedPacket() { _missedPacket = 0UL; };
            void setRoomId(std::uint64_t roomId) { _roomId = roomId; };

        private:
            std::string _uuid;
            asio::ip::udp::endpoint _endpoint;
            std::size_t _missedPacket;
            std::uint64_t _roomId;
    };

    class Network
    {
        public:
            Network();
            ~Network();

            const asio::io_context &getIoContext() const noexcept {return _ioContext;}
            const asio::io_context &getIoService() const noexcept {return _ioService;}
            const std::string &getHost() const noexcept {return _host;}
            std::uint16_t getPort() const noexcept {return _port;}
            const asio::ip::udp::endpoint &getServerEndpoint() const noexcept {return _serverEndpoint;}
            const asio::ip::udp::socket &getSocket() const noexcept {return _socket;}
            const asio::error_code &getAsioErrorCode() const noexcept {return _errCode;}
            std::vector<Client> &getClients() noexcept {return _clients;}
            const std::array<std::uint8_t, packetSize> &getPacket() const noexcept {return _packet;}

            void setHost(const std::string &host) {_host = host;}
            void setPort(std::uint16_t port) {_port = port;}
            void setConnection(const std::string &host, std::uint16_t port) {_host = host; _port = port;}
            static void setServerInstance(Network *instance) {serverInstance = instance;}

            void writeToLogs(const std::string_view &status, const std::string &msg)
            {
                _logs.logTo(status.data(), msg.c_str());
            }

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
            void sendResponse(const packet::packetTypes &type, T &data, std::uint64_t roomId = std::numeric_limits<std::uint64_t>::max(), bool toServerEndpoint = false, const std::string cliUuid = "")
            {
                packet::packetHeader header(type, sizeof(data), false, 0UL, roomId);
                std::array<std::uint8_t, sizeof(header) + sizeof(data)> buffer;
                std::memmove(&buffer, &header, sizeof(header));
                std::memmove(&buffer[sizeof(header)], &data, header.dataSize);
                if (toServerEndpoint) {
                    _socket.send_to(asio::buffer(&buffer, sizeof(buffer)), _serverEndpoint);
                    return;
                }
                else if (cliUuid.empty() && !_clients.empty() && roomId == std::numeric_limits<std::uint64_t>::max())
                    _logs.logTo(logInfo.data(), "Sending packet type [" + std::to_string(header.type) + "] to all clients:");
                for (Client &client : _clients) {
                    if (!cliUuid.empty()) {
                        if (cliUuid == client.getUuid() && client.getRoomId() == roomId) {
                            _socket.send_to(asio::buffer(&buffer, sizeof(buffer)), client.getEndpoint());
                            _logs.logTo(logInfo.data(), "Sent packet type [" + std::to_string(header.type) + "] to [" + cliUuid + "] in room [" + std::to_string(client.getRoomId()) + "]");
                            return;
                        }
                    } else if (client.getRoomId() == roomId) {
                        _socket.send_to(asio::buffer(&buffer, sizeof(buffer)), client.getEndpoint());
                        _logs.logTo(logInfo.data(), "    Sent packet type [" + std::to_string(header.type) + "] to [" + client.getUuid() + "] in room [" + std::to_string(client.getRoomId()) + "]");
                    }
                }
            }

            template<class T>
            void sendSparseArray(const packet::packetTypes &type, sparse_array<T> &sparseArray, std::uint64_t roomId, const std::string cliUuid = "")
            {
                const std::size_t componentSize = sizeof(T);
                const std::size_t componentsSize = (sizeof(bool) + componentSize) * sparseArray.size();
                const std::size_t headerSize = sizeof(packet::packetHeader);
                const std::size_t totalSize = headerSize + componentsSize;

                packet::packetHeader header(type, componentsSize, true, 0U, roomId);
                std::vector<std::uint8_t> buffer(totalSize);
                std::size_t offset = 0UL;
                zlib::ZLib z;

                for (auto &component : sparseArray) {
                    if (component.has_value()) {
                        bool isNullOpt = false;
                        std::memmove(buffer.data() + offset, &isNullOpt, sizeof(bool));
                        offset += sizeof(bool);
                        std::memmove(buffer.data() + offset, &component.value(), componentSize);
                    } else {
                        bool isNullOpt = true;
                        std::memmove(buffer.data() + offset, &isNullOpt, sizeof(bool));
                        offset += sizeof(bool);
                    }
                    offset += componentSize;
                }
                std::vector<std::uint8_t> compressedBuf;
                int result = z.compress(compressedBuf, header.compressedSize, buffer, Z_BEST_COMPRESSION);
                if (result == Z_OK) {
                    const std::size_t compressedDiff = buffer.size() - compressedBuf.size();
                    const std::size_t compressedSize = compressedBuf.size();
                    std::vector<std::uint8_t> packetData(headerSize + compressedSize);
                    std::memmove(packetData.data(), &header, headerSize);
                    std::memmove(packetData.data() + headerSize, compressedBuf.data(), compressedSize);
                    for (Client &client : _clients) {
                        if (!cliUuid.empty()) {
                            if (cliUuid == client.getUuid() && client.getRoomId() == roomId) {
                                _socket.send_to(asio::buffer(buffer.data(), totalSize), client.getEndpoint());
                                return;
                            }
                        } else if (client.getRoomId() == roomId) {
                            _socket.send_to(asio::buffer(packetData.data(), headerSize + compressedSize), client.getEndpoint());
                        }
                    }
                    (void)compressedDiff; // Remove if next line used
                    //_logs.logTo(logInfo.data(), "Game update sent (compressed by " + std::to_string(compressedDiff) + " bytes.)");
                } else
                    std::cerr << "Compression failed with error code: " << result << std::endl;
            }

            void receiveCallback(const asio::error_code &errCode, std::size_t bytesReceived);
            void receive();
            void startServer();
            void stopServer();

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

            std::string addClient(std::uint64_t roomId);

            bool removeClient(const std::string &uuid);

        private:
            asio::io_context _ioContext;
            asio::io_service _ioService;
            std::string _host;
            std::uint16_t _port;
            asio::ip::udp::endpoint _serverEndpoint;
            asio::ip::udp::socket _socket;
            asio::error_code _errCode;
            std::vector<std::thread> _threadPool;
            std::array<std::uint8_t, packetSize> _packet;
            Log _logs;
            std::vector<Client> _clients;
            static Network* serverInstance;
    };

    inline Network* Network::serverInstance = nullptr;
}

#endif /* !NETWORK_HPP_ */