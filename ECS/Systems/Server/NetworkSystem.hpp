/*
** EPITECH PROJECT, 2023
** R-Type
** File description:
** NetworkSystem
*/

#ifndef NETWORKSYSTEM_HPP_
#define NETWORKSYSTEM_HPP_

#include "SparseArray.hpp"
#include "Registry.hpp"
#include "Network.hpp"
#include "Room.hpp"
#include "Score.hpp"

namespace rtype
{
    class NetworkSystem
    {
    public:
        NetworkSystem() : _net() {}
        ~NetworkSystem() = default;

        std::pair<float, float> newPlayer(std::uint64_t roomId, Registry &ecs, const std::string &clientUUID)
        {
            float posX = 30.0f;
            float posY = 250.0f * getConnectedNb(roomId);
            Position position(posX, posY);
            Controllable ctrl(clientUUID);
            Hitbox hb(33, 17);
            Entity entity = ecs.spawn_entity();
            ecs.add_component<Position>(entity, position);
            ecs.add_component<Velocity>(entity, 3);
            ecs.add_component<Controllable>(entity, ctrl);
            ecs.add_component<Hitbox>(entity, hb);
            ecs.add_component<Destroyable>(entity, 5);
            ecs.add_component<Tag>(entity, {TagEnum::PLAYER});
            _net.writeToLogs(logGameInfo, "Entity has been summoned at " + std::to_string(posX) + "f " + std::to_string(posY) + "f.");
            return std::make_pair(posX, posY);
        }

        void removePlayer(Registry &ecs, const std::string &clientUUID)
        {
            for (auto &component : ecs.get_components<Controllable>())
            {
                std::string playId(uuidSize, 0);
                std::memmove(playId.data(), &component->_playerId, uuidSize);
                if (component.has_value() && !std::strcmp(playId.c_str(), clientUUID.c_str()))
                {
                    ecs.get_component<Destroyable>(Entity(ecs.get_components<Controllable>().get_index(component)))._hp = 0;
                    _net.writeToLogs(logGameInfo, "Entity has been killed.");
                }
            }
        }

        void affectControllable(Registry &ecs, const std::string &clientUUID, int keyCode)
        {
            auto &conts = ecs.get_components<Controllable>();
            for (auto &cont : conts)
            {
                std::string playId(uuidSize, 0);
                std::memmove(playId.data(), &cont->_playerId, uuidSize);
                if (cont && !std::strcmp(playId.c_str(), clientUUID.c_str()))
                {
                    cont->latestInput = keyCode;
                }
            }
        }

        bool addClientToRoom(const std::string &clientUUID, std::uint64_t roomId)
        {
            std::size_t clientsInRoomNb = 0UL;
            for (auto &room : _rooms) {
                clientsInRoomNb = room.getClients().size();
                if (room.getId() == roomId && static_cast<std::uint64_t>(room.getMaxSlots()) > clientsInRoomNb) {
                    room.addClient(clientUUID);
                    _net.writeToLogs(logInfo, "Client connected to room " + std::to_string(roomId));
                    return true;
                } else if (room.getId() == roomId && static_cast<std::uint64_t>(room.getMaxSlots()) <= clientsInRoomNb) {
                    _net.writeToLogs(logWarn, "Client refused to room " + std::to_string(roomId) + ": Full");
                    return false;
                }
            }
            _net.writeToLogs(logErr, "Client could not connected to room " + std::to_string(roomId) + ": Does not exist.");
            return false;
        }

        bool removeClientFromRoom(const std::string &clientUUID, std::uint64_t roomId)
        {
            if (roomId < _rooms.size()) {
                std::vector<std::string> &clients = _rooms[roomId].getClients();
                auto it = std::find(clients.begin(), clients.end(), clientUUID.data());
                if (it != clients.end()) {
                    clients.erase(it);
                    _net.writeToLogs(logInfo, "Client removed from room " + std::to_string(roomId));
                    if (_rooms[roomId].getClients().size() == 0UL) {
                        deleteRoom(roomId);
                        _net.writeToLogs(logInfo, "Room " + std::to_string(roomId) + " closed: Empty");
                        packet::roomClosed roomClosed(roomId);
                        _net.sendResponse(packet::ROOM_CLOSED, roomClosed);
                    }
                    return true;
                }
                _net.writeToLogs(logErr, "Client not found in any available rooms.");
            } else
                _net.writeToLogs(logErr, "Room " + std::to_string(roomId) + " does not exist");
            return false;
        }

        void handleTextMessage(const std::array<std::uint8_t, packetSize> &packet, const packet::packetHeader &header)
        {
            std::string text(256UL, 0);
            std::string clientUUID(uuidSize, 0);
            packet::textMessage txtmsg;
            std::memmove(&txtmsg, &packet[sizeof(header)], sizeof(txtmsg));
            std::memmove(clientUUID.data(), txtmsg.uuid.data(), uuidSize);
            std::memmove(text.data(), &txtmsg.message, txtmsg.msgSize);
            if (clientUUID.empty() || clientUUID[0] == 0) {
                _net.writeToLogs(logGameErr, "Received a corrupted message. UUID is empty or unknown.");
            } else if (text.empty() || text[0] == 0)
                _net.writeToLogs(logGameErr, "Received a corrupted message. Message is empty.");
            else {
                _net.writeToLogs(logGameInfo, clientUUID + ": " + text.data());
                _net.sendResponse(packet::TEXT_MESSAGE, txtmsg, header.roomId);
            }
        }

        bool roomExist(std::uint64_t roomId)
        {
            for (auto &room : _rooms)
                if (room.getId() == roomId)
                    return true;
            return false;
        }

        std::uint64_t createNewRoom(std::uint8_t maxSlots)
        {
            std::uint64_t roomId = _rooms.size();
            if (maxSlots < 2U) {
                _net.writeToLogs(logWarn, "Room max slots increased to 3. Got " + std::to_string(maxSlots));
                maxSlots = 3U;
            }
            _rooms.emplace_back(Room(roomId, maxSlots));
            _net.writeToLogs(logInfo, "New room of " + std::to_string(maxSlots) + " clients created. ID " + std::to_string(roomId));
            return roomId;
        }

        void deleteRoom(std::uint64_t roomId)
        {
            std::vector<Room>::iterator it = std::remove_if(_rooms.begin(), _rooms.end(),
                [&](const Room &room) {
                    return room.getId() == roomId;
                }
            );

            if (it != _rooms.end()) {
                _rooms.erase(it, _rooms.end());
            }
        }

        template<typename T>
        void sendResponse(const packet::packetTypes &type, T &data, std::uint64_t roomId = std::numeric_limits<std::uint64_t>::max(), bool toServerEndpoint = false, const std::string cliUuid = "")
        {
            _net.sendResponse(type, data, roomId, toServerEndpoint, cliUuid);
        }

        template <class T>
        void sendSparseArray(const packet::packetTypes &type, sparse_array<T> &sparseArray, std::uint64_t roomId, const std::string &cliUuid = "")
        {
            _net.sendSparseArray<T>(type, sparseArray, roomId, cliUuid);
        }

        bool isServerAvailable()
        {
            return _net.isSocketOpen();
        }

        std::size_t getConnectedNb(std::uint64_t roomId)
        {
            for (auto &room : _rooms) {
                if (room.getId() == roomId)
                    return room.getClients().size();
            }
            return 0UL;
        }

        std::vector<Room> &getRooms() noexcept
        {
            return _rooms;
        }

        void initRoom(std::uint64_t roomId, std::unordered_map<std::uint64_t, Registry> &regs)
        {
            regs[roomId] = Registry();
            regs[roomId].register_component<Position>();
            regs[roomId].register_component<Velocity>();
            regs[roomId].register_component<Hitbox>();
            regs[roomId].register_component<Controllable>();
            regs[roomId].register_component<MovementPattern>();
            regs[roomId].register_component<Destroyable>();
            regs[roomId].register_component<Damaging>();
            regs[roomId].register_component<Score>();
            regs[roomId].register_component<Tag>();
            regs[roomId].spawn_entity(); // Background index
            regs[roomId].spawn_entity(); // ChatBox index
        }

        void handleConnectionRequest(std::unordered_map<std::uint64_t, Registry> &regs, packet::packetTypes type, std::array<std::uint8_t, packetSize> &packet)
        {
            packet::connectionRequest request;
            std::uint64_t roomId = request.roomId;
            std::memmove(&request, &packet[sizeof(packet::packetHeader)], sizeof(request));
            std::string clientUUID = _net.addClient(roomId);
            if (roomExist(roomId)) {
                if (!addClientToRoom(clientUUID, roomId)) {
                    packet::connectionRequest data(packet::REJECTED);
                    _net.sendResponse(packet::CONNECTION_REQUEST, data, roomId, true);
                    removeClient(clientUUID);
                    return;
                }
            } else {
                if (request.createRoom) {
                    roomId = createNewRoom(5UL);
                    initRoom(roomId, regs);
                    if (!addClientToRoom(clientUUID, roomId)) {
                        packet::connectionRequest data(packet::REJECTED);
                        _net.sendResponse(packet::CONNECTION_REQUEST, data, roomId, true);
                        removeClient(clientUUID);
                        return;
                    }
                } else {
                    packet::connectionRequest data(packet::REJECTED);
                    _net.sendResponse(packet::CONNECTION_REQUEST, data, roomId, true);
                    removeClient(clientUUID);
                    return;
                }
            }
            packet::joinedRoom joinedRoom(clientUUID, roomId);
            _net.sendResponse(packet::JOINED_ROOM, joinedRoom, roomId);
            packet::connectionRequest data(packet::ACCEPTED, clientUUID, getConnectedNb(roomId));
            std::pair<float, float> pos = newPlayer(roomId, regs[roomId], clientUUID);
            _net.writeToLogs(logInfo, "UUID of player is " + clientUUID);
            _net.writeToLogs(logInfo, "Connected!");
            _net.sendResponse(packet::CONNECTION_REQUEST, data, roomId, true);
            packet::clientStatus cliStatus(clientUUID, packet::NEW_CLIENT, pos.first, pos.second, getConnectedNb(roomId));
            try {
                _net.sendResponse(packet::CLIENT_STATUS, cliStatus, roomId);
            } catch (const std::system_error &e) {
                std::string err = e.what();
                _net.writeToLogs(logWarn, "Failed to send the response of packet type [" + std::to_string(type) + "]:");
                _net.writeToLogs(logWarn, "    " + err);
            }
        }

        void handleDisconnectionRequest(std::unordered_map<std::uint64_t, Registry> &regs, const std::array<std::uint8_t, packetSize> &packet, const packet::packetHeader &header)
        {
            packet::disconnectionRequest request(packet::ACCEPTED, 0UL);
            std::memmove(&request, &packet[sizeof(header)], sizeof(request));
            std::string clientUUID(uuidSize, 0);
            std::memmove(clientUUID.data(), &request.uuid, uuidSize);
            if (!std::strncmp(clientUUID.data(), "\0", 1UL)) {
                _net.writeToLogs(logErr, "Could not disconnect the client, no UUID provided.");
                return;
            }
            if (!_net.removeClient(clientUUID)) {
                _net.writeToLogs(logErr, "Could not disconnect the client, UUID is unknown.");
                return;
            }
            if (!roomExist(request.roomId)) {
                _net.writeToLogs(logErr, "Could not disconnect the client from unknown room ID: " + std::to_string(request.roomId));
                return;
            }
            removePlayer(regs[request.roomId], clientUUID);
            removeClientFromRoom(clientUUID, request.roomId);
            packet::leftRoom leftRoom(clientUUID, request.roomId);
            _net.sendResponse(packet::LEFT_ROOM, leftRoom, request.roomId);
            _net.writeToLogs(logInfo, "Lost connection: Disconnected " + clientUUID);
            packet::clientStatus cliStatus(clientUUID, packet::LOSE_CLIENT, 0.0f, 0.0f, getConnectedNb(request.roomId));
            try {
                _net.sendResponse(packet::CLIENT_STATUS, cliStatus, request.roomId);
            } catch (const std::system_error &e) {
                std::string err = e.what();
                _net.writeToLogs(logWarn, "Failed to send the response of packet type [" + std::to_string(header.type) + "]:");
                _net.writeToLogs(logWarn, "    " + err);
            }
        }

        void handleEntityKilled(Entity entity, std::uint64_t roomId)
        {
            packet::entityKilledStatus status(entity());
            try {
                _net.sendResponse(packet::ENTITY_KILLED, status, roomId);
            } catch (const std::system_error &e) {
                std::string err = e.what();
                _net.writeToLogs(logWarn, "    " + err);
            }
        }

        void handlekeyboardEvent(std::unordered_map<std::uint64_t, Registry> &regs, const std::array<std::uint8_t, packetSize> &packet, const packet::packetHeader &header)
        {
            packet::keyboardEvent event;
            std::memmove(&event, &packet[sizeof(header)], sizeof(event));
            std::string clientUUID(uuidSize, 0);
            std::memmove(clientUUID.data(), &event.uuid, uuidSize);
            affectControllable(regs[event.roomId], clientUUID, event.keyCode);
        }

        void handleCreateRoom(std::unordered_map<std::uint64_t, Registry> &regs, const std::array<std::uint8_t, packetSize> &packet)
        {
            packet::createRoom createRoom(0UL);
            std::memmove(&createRoom, &packet[sizeof(packet::packetHeader)], sizeof(createRoom));
            std::uint64_t roomId = createNewRoom(createRoom.maxSlots);
            initRoom(roomId, regs);
            packet::roomAvailable roomAvailable(roomId, _rooms[roomId].getMaxSlots());
            _net.sendResponse(packet::ROOM_AVAILABLE, roomAvailable);
        }

        void networkSystemServer(std::unordered_map<std::uint64_t, Registry> &regs)
        {
            std::array<std::uint8_t, packetSize> packet;
            packet::packetHeader header;
            std::unordered_map<packet::packetTypes, std::function<void()>> dataHandlers = {
                {packet::PLACEHOLDER, [&] { _net.writeToLogs(logInfo, "Placeholder received successfully."); }},
                {packet::CONNECTION_REQUEST, [&] { handleConnectionRequest(regs, header.type, packet); }},
                {packet::DISCONNECTION_REQUEST, [&] { handleDisconnectionRequest(regs, packet, header); }},
                {packet::KEYBOARD_EVENT, [&] { handlekeyboardEvent(regs, packet, header); }},
                {packet::KEEP_CONNECTION, [&] { handleKeepConnection(packet, header); }},
                {packet::TEXT_MESSAGE, [&] { handleTextMessage(packet, header); }},
                {packet::CREATE_ROOM, [&] { handleCreateRoom(regs, packet); }},
            };

            _net.startServer();
            while (_net.isSocketOpen()) {
                packet = _net.getPacket();
                header = *reinterpret_cast<packet::packetHeader *>(packet.data());
                if (header.type != packet::NONE) {
                    auto handlerIt = dataHandlers.find(header.type);
                    if (handlerIt != dataHandlers.end()) {
                        handlerIt->second();
                    } else {
                        _net.writeToLogs(logWarn, "Unknown packet type " + std::to_string(header.type));
                    }
                }
            }
        }

        std::vector<net::Client> &getClients() noexcept
        {
            return _net.getClients();
        }

        void removeClient(const std::string &clientUUID)
        {
            _net.removeClient(clientUUID);
        }

        void writeToLogs(const std::string_view &status, const std::string &msg)
        {
            _net.writeToLogs(status, msg);
        }

        bool removeTimeoutClients(std::uint64_t roomId, Registry &ecs)
        {
            packet::keepConnection data;
            sendResponse(packet::KEEP_CONNECTION, data, roomId);
            for (auto &client : getClients()) {
                client.packetMissed();
                std::string clientUUID = client.getUuid();
                if (client.getMissedPacket() >= 10UL) {
                    writeToLogs(logInfo, "Client " + clientUUID + " connection timeout.");
                    packet::clientStatus cliStatus(clientUUID, packet::LOSE_CLIENT, 0.0f, 0.0f, getConnectedNb(roomId) - 1);
                    sendResponse(packet::CLIENT_STATUS, data, roomId);
                    removePlayer(ecs, clientUUID);
                    removeClient(clientUUID);
                    return true;
                }
            }
            return false;
        }

        void handleKeepConnection(const std::array<std::uint8_t, packetSize> &packet, const packet::packetHeader &header)
        {
            std::string clientUUID(uuidSize, 0);
            packet::keepConnection data;

            std::memmove(&data, &packet[sizeof(header)], sizeof(data));
            std::memmove(clientUUID.data(), &data.uuid, uuidSize);

            auto &clients = _net.getClients();
            
            for (auto &client : clients) {
                if (client.getUuid() == clientUUID.data()) {
                    client.resetMissedPacket();
                    _net.writeToLogs(logInfo, "Response to connection check received from " + clientUUID);
                    return;
                }
            }
            _net.writeToLogs(logErr, "Connection check failed, did not find UUID " + clientUUID);
        }

        net::Network _net;
        std::vector<Room> _rooms;
    };
}

#endif /* !NETWORKSYSTEM_HPP_ */
