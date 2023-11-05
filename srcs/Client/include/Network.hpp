#ifndef NETWORK_HPP_
#define NETWORK_HPP_

#include <string>
#include <cstdint>
#include <asio.hpp>
#include <iostream>
#include <csignal>
#include <functional>
#include <utility>
#include "CFGParser.hpp"
#include "Packets.hpp"
#include "SparseArray.hpp"
#include "Velocity.hpp"
#include "Position.hpp"
#include "Hitbox.hpp"
#include "Texture.hpp"
#include "Registry.hpp"
#include "Controllable.hpp"
#include "Scale.hpp"
#include "MovementPattern.hpp"
#include "Destroyable.hpp"
#include "Damaging.hpp"
#include "Score.hpp"
#include "Tag.hpp"
#include "ZLib.hpp"
#include "ChatBox.hpp"

// Default values used if parsing fails or invalid values are set.
static constexpr std::string_view defaultHost = "127.0.0.1";
static constexpr std::uint16_t defaultPort = 12345U;
static constexpr std::uint8_t defaultTimeout = 5U;
static constexpr std::uint32_t localPacketSize = 1024U;

#ifdef _WIN32
static constexpr std::string_view clientConfigFilePath = "\\resources\\client.cfg";
#else
static constexpr std::string_view clientConfigFilePath = "/resources/client.cfg";
#endif /* !_WIN32 */

namespace net
{
    class Client
    {
        public:
            Client() = delete;
            Client(asio::io_context &ioContext, const std::string &host, const std::string &port, Registry &reg) :
                _ioContext(ioContext), _errCode(asio::error_code()), _resolver(ioContext), _endpoint(*_resolver.resolve(asio::ip::udp::v4(), host, port).begin()),
                _socket(asio::ip::udp::socket(ioContext)), _timer(ioContext), _uuid(uuidSize, 0), _roomId(0UL), _packet(localPacketSize), _reg(reg)
            {
            };
            ~Client()
            {
                for (auto &thread : clientInstance->_threadPool) {
                    if (thread.joinable()) {
                        thread.join();
                    }
                }
                _socket.close();
            };

            void run()
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
                _threadPool.emplace_back([&]() {
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
                    packet::pingRequest request;
                    packet::packetHeader header(packet::PING_REQUEST, sizeof(request));
                    std::size_t bytesSent = sendPacket(header, request);
                    if (bytesSent == 0UL)
                        std::cerr << "Something went wrong sending the packet ping." << std::endl;
                    _ioContext.run();
                });
            }

            bool isSocketOpen() const noexcept
            {
                return _socket.is_open();
            }

            static void setClientInstance(Client *instance) { clientInstance = instance; }

            template <typename T>
            std::size_t sendPacket(packet::packetHeader &header, T &data)
            {
                const std::size_t headerSize = sizeof(header);
                const std::size_t dataSize = sizeof(data);
                std::array<std::uint8_t, headerSize + dataSize> packet;

                std::memmove(&packet, &header, headerSize);
                std::memmove(&packet[headerSize], &data, dataSize);
                if (_socket.is_open())
                    return _socket.send_to(asio::buffer(&packet, headerSize + dataSize), _endpoint);
                return 0UL;
            }

            void listenServer()
            {
                try
                {
                    _socket.async_receive_from(
                        asio::buffer(_packet.data(), _packet.size()),
                        _endpoint,
                        std::bind(&Client::handleReceive, this, std::placeholders::_1));
                }
                catch (const asio::system_error &e)
                {
                    std::cerr << "Failed to receive packet: " << e.what() << std::endl;
                }
                catch (const Error &)
                {
                    std::cerr << "Server connection closed" << std::endl;
                }
            }

            void handlePlaceholderPacket()
            {
                std::cout << "Received a placeholder packet from server" << std::endl;
            }

            void handleConnectionRequestPacket(const packet::connectionRequest &request) {
                if (request.status == packet::ACCEPTED) {
                    std::memmove(_uuid.data(), &request.uuid, uuidSize);
                    _roomId = request.roomId;
                    std::cout << "Got uuid = " << _uuid << std::endl;
                    std::cout << request.connectedNb << " client connected." << std::endl;
                } else {
                    std::cout << "rejected" << std::endl;
                }
            }

            void handleClientStatusPacket(packet::clientStatus &cliStatus)
            {
                std::string cliUuid(reinterpret_cast<char *>(cliStatus.uuid.data()));
                if (cliStatus.status == packet::LOSE_CLIENT && std::strcmp(cliUuid.c_str(), _uuid.c_str()))
                {
                    std::cout << "Client " << cliUuid << " disconnected." << std::endl;
                    std::cout << cliStatus.connectedNb << " clients connected." << std::endl;
                }
                else if (cliStatus.status == packet::NEW_CLIENT && std::strcmp(cliUuid.c_str(), _uuid.c_str()))
                {
                    std::cout << "Client " << cliUuid << " connected at X: " << cliStatus.posX << " Y: " << cliStatus.posY << std::endl;
                    std::cout << cliStatus.connectedNb << " clients connected." << std::endl;
                }
            }

            void handleForceDisconnectPacket()
            {
                _socket.cancel();
                _ioContext.stop();
                _socket.close();
                _uuid.clear();
            }

            template <class T, typename U>
            void handleECSComponent(packet::packetHeader &header, U &component)
            {
                const std::size_t componentSize = sizeof(T);
                const std::size_t headerSize = sizeof(header);
                auto &arr = _reg.get_components<T>();

                bool isNullOpt = false;
                std::size_t sparseArrIndex = 0UL;
                if (header.compressed) {
                    std::vector<std::uint8_t> decompressedData(header.dataSize);
                    std::vector<std::uint8_t> compressedData(header.compressedSize);
                    std::memmove(compressedData.data(), _packet.data() + headerSize, header.compressedSize);
                    zlib::ZLib z;
                    int result = z.decompress(decompressedData, compressedData, localPacketSize);
                    if (result == Z_OK) {
                        for (std::size_t componentIdx = 0UL; componentIdx < header.dataSize;) {
                            std::memmove(&isNullOpt, decompressedData.data() + componentIdx, sizeof(bool));
                            componentIdx += sizeof(bool);
                            if (!isNullOpt) {
                                std::memmove(&component, decompressedData.data() + componentIdx, componentSize);
                                if (sparseArrIndex == arr.size()) {
                                    _reg.spawn_entity();
                                }
                                arr[sparseArrIndex] = component;
                            }
                            componentIdx += componentSize;
                            sparseArrIndex += 1UL;
                        }
                    }
                }
            }

            void handleKeepConnection()
            {
                packet::packetHeader header(packet::KEEP_CONNECTION, uuidSize);
                packet::keepConnection data(_uuid);
                sendPacket(header, data);
            }

            void handleRoomAvailable(const packet::roomAvailable &data)
            {
                _rooms.emplace_back(std::make_pair(data.roomId, data.maxSlots));
                std::cout << "New room available for " << std::to_string(data.maxSlots) << " players with ID " << std::to_string(data.roomId) << std::endl;
            }

            void handleNewClientInRoom(const packet::joinedRoom &data)
            {
                std::string clientUUID(uuidSize, 0);
                std::memmove(clientUUID.data(), &data.uuid, uuidSize);
                std::cout << "New client in room " << data.roomId << std::endl;
                std::cout << "UUID of player is " << clientUUID << std::endl;
            }

            void handleClientLeftRoom(const packet::leftRoom &data)
            {
                std::string clientUUID(uuidSize, 0);
                std::memmove(clientUUID.data(), &data.uuid, uuidSize);
                std::cout << "Client left room " << data.roomId << std::endl;
                std::cout << "UUID of player who left is " << clientUUID << std::endl;
            }

            void handleClientRoomClosed(const packet::roomClosed &data)
            {
                std::cout << "Room " << std::to_string(data.roomId) << " has been closed" << std::endl;
            }

            void handleTextMessage(const packet::textMessage &data)
            {
                std::string clientUUID(uuidSize, 0);
                std::string message(data.msgSize, 0);
                std::memmove(clientUUID.data(), &data.uuid, uuidSize);
                std::memmove(message.data(), &data.message, data.msgSize);
                if (clientUUID.empty()) {
                    std::cerr << "Got a message from an unknown client UUID or corrupted." << std::endl;
                } else if (message.empty())
                    std::cerr << "Got an empty message or corrupted." << std::endl;
                else {
                    for (int i = 0; i < _reg.get_entity_number(); i++) {
                        if (_reg.entity_has_component<ChatBox>(Entity(i))) {
                            auto &chatBox = _reg.get_component<ChatBox>(Entity(i));
                            chatBox.addMessage(message);
                        }
                    }
                }
            }

            void handleReceive(const asio::error_code &errCode)
            {
                packet::packetHeader header;
                std::size_t headerSize = sizeof(header);

                if (!errCode) {
                    std::memmove(&header, _packet.data(), headerSize);
                    std::unordered_map<std::uint16_t, std::function<void()>> packetHandlers = {
                        {packet::PLACEHOLDER, [&]{ handlePlaceholderPacket(); }},
                        {packet::CONNECTION_REQUEST, [&]{
                            packet::connectionRequest request;
                            std::memmove(&request, _packet.data() + headerSize, header.dataSize);
                            handleConnectionRequestPacket(request);
                        }},
                        {packet::CLIENT_STATUS, [&]{
                            packet::clientStatus cliStatus;
                            std::memmove(&cliStatus, _packet.data() + headerSize, sizeof(cliStatus));
                            handleClientStatusPacket(cliStatus);
                        }},
                        {packet::FORCE_DISCONNECT, [&]{ handleForceDisconnectPacket(); }},
                        {packet::ECS_VELOCITY, [&]{
                            Velocity component(0);
                            handleECSComponent<Velocity>(header, component);
                        }},
                        {packet::ECS_CONTROLLABLE, [&]{
                            Controllable component("");
                            handleECSComponent<Controllable>(header, component);
                        }},
                        {packet::ECS_POSITION, [&]{
                            Position component(0.0f, 0.0f);
                            handleECSComponent<Position>(header, component);
                        }},
                        {packet::ECS_HITBOX, [&]{
                            Hitbox component(0, 0);
                            handleECSComponent<Hitbox>(header, component);
                        }},
                        {packet::ECS_DESTROYABLE, [&]{
                            Destroyable component(0);
                            handleECSComponent<Destroyable>(header, component);
                        }},
                        {packet::ECS_MOVEMENTPATTERN, [&]{
                            MovementPattern component(MovementPatterns::NONE);
                            handleECSComponent<MovementPattern>(header, component);
                        }},
                        {packet::ECS_DAMAGES, [&]{
                            Damaging component(0);
                            handleECSComponent<Damaging>(header, component);
                        }},
                        {packet::ECS_SCORE, [&]{
                            Score component(0);
                            handleECSComponent<Score>(header, component);
                        }},
                        {packet::TEXT_MESSAGE, [&]{
                            packet::textMessage txtmsg;
                            std::memmove(&txtmsg, _packet.data() + headerSize, sizeof(txtmsg));
                            handleTextMessage(txtmsg);
                        }},
                        {packet::KEEP_CONNECTION, [&]{
                            handleKeepConnection();
                        }},
                        {packet::ROOM_AVAILABLE, [&]{
                            packet::roomAvailable roomAvailable;
                            std::memmove(&roomAvailable, _packet.data() + headerSize, sizeof(roomAvailable));
                            handleRoomAvailable(roomAvailable);
                        }},
                        {packet::JOINED_ROOM, [&]{
                            packet::joinedRoom joinedRoom;
                            std::memmove(&joinedRoom, _packet.data() + headerSize, sizeof(joinedRoom));
                            handleNewClientInRoom(joinedRoom);
                        }},
                        {packet::LEFT_ROOM, [&]{
                            packet::leftRoom leftRoom;
                            std::memmove(&leftRoom, _packet.data() + headerSize, sizeof(leftRoom));
                            handleClientLeftRoom(leftRoom);
                        }},
                        {packet::ROOM_CLOSED, [&]{
                            packet::roomClosed roomClosed;
                            std::memmove(&roomClosed, _packet.data() + headerSize, sizeof(roomClosed));
                            handleClientRoomClosed(roomClosed);
                        }},
                        {packet::ECS_TAG, [&]{
                            Tag component(TagEnum::NOTAG);
                            handleECSComponent<Tag>(header, component);
                        }},
                        {packet::ENTITY_KILLED, [&]{
                            std::uint32_t entityId;
                            std::memmove(&entityId, _packet.data() + headerSize, sizeof(entityId));
                            _reg.kill_entity(Entity(entityId));
                        }},
                    };

                    auto handlerIt = packetHandlers.find(header.type);
                    if (handlerIt != packetHandlers.end()) {
                        if (header.roomId == std::numeric_limits<std::uint64_t>::max() || header.roomId == _roomId)
                            handlerIt->second();
                    } else {
                        std::cerr << "Unknown packet" << std::endl;
                    }
                }
                _timer.cancel();
                if (!errCode && header.type != packet::FORCE_DISCONNECT) {
                    listenServer();
                }
            }

            void roomList()
            {
                _rooms.clear();
                packet::roomListRequest request;
                packet::packetHeader header(packet::ROOM_LIST_REQUEST, sizeof(request));
                std::size_t bytesSent = sendPacket(header, request);
                if (bytesSent == 0UL)
                    std::cerr << "Something went wrong sending the packet room list." << std::endl;
            }

            std::vector<std::pair<std::uint64_t, std::uint8_t>> getRooms() const noexcept
            {
                return _rooms;
            }

            void connect(int roomId, bool createRoom)
            {
                packet::connectionRequest request(roomId, createRoom);
                packet::packetHeader header(packet::CONNECTION_REQUEST, sizeof(request));
                std::size_t bytesSent = sendPacket(header, request);
                if (bytesSent == 0UL)
                    std::cerr << "Something went wrong sending the packet connection." << std::endl;
            }

            void disconnect()
            {
                packet::disconnectionRequest request(_uuid, _roomId);
                packet::packetHeader header(packet::DISCONNECTION_REQUEST, sizeof(request));
                if (_socket.is_open())
                {
                    std::size_t bytesSent = sendPacket(header, request);
                    if (bytesSent == 0UL)
                        std::cerr << "Something went wrong sending the packet disconnection." << std::endl;
                }
                std::cout << "Disconnected from the server." << std::endl;
                _uuid.clear();
                _ioContext.stop();
                _socket.close();
            }

            static void signalHandler(int signum)
            {
                if (signum == SIGINT && clientInstance) {
                    if (!clientInstance->_uuid.empty())
                        clientInstance->disconnect();
                }
                std::exit(EXIT_SUCCESS);
            }

            const std::string getUuid() const { return _uuid; }

            std::uint64_t getRoomId() const noexcept { return _roomId; }

        private:
            asio::io_context &_ioContext;
            asio::error_code _errCode;
            asio::ip::udp::resolver _resolver;
            asio::ip::udp::endpoint _endpoint;
            asio::ip::udp::socket _socket;
            asio::steady_timer _timer;
            std::string _uuid;
            std::uint64_t _roomId;
            std::vector<std::thread> _threadPool;
            std::vector<std::uint8_t> _packet;
            Registry &_reg;
            static Client* clientInstance;
            std::vector<std::pair<std::uint64_t, std::uint8_t>> _rooms;
    };

    Client *Client::clientInstance = nullptr;
} // namespace net

#endif /* !NETWORK_HPP_ */