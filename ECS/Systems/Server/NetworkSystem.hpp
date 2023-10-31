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

namespace rtype
{
    class NetworkSystem
    {
        public:
            NetworkSystem() : _net() {}
            ~NetworkSystem() = default;

            std::pair<float, float> newPlayer(Registry &ecs, const std::string &clientUUID)
            {
                float posX = 30.0f;
                float posY = 250.0f * getConnectedNb();
                Position position(posX, posY);
                Controllable ctrl(clientUUID);
                Hitbox hb(99, 51);
                Entity entity = ecs.spawn_entity();
                ecs.add_component<Position>(entity, position);
                ecs.add_component<Velocity>(entity, 3);
                ecs.add_component<Controllable>(entity, ctrl);
                ecs.add_component<Hitbox>(entity, hb);
                ecs.add_component<Destroyable>(entity, 1);
                _net.writeToLogs(logGameInfo, "Entity has been summoned at " + std::to_string(posX) + "f " + std::to_string(posY) + "f.");
                return std::make_pair(posX, posY);
            }

            void removePlayer(Registry &ecs, const std::string &clientUUID)
            {
                for (auto &component : ecs.get_components<Controllable>()) {
                    std::string playId(uuidSize, 0);
                    std::memmove(playId.data(), &component->_playerId, uuidSize);
                    if (component.has_value() && !std::strcmp(playId.c_str(), clientUUID.c_str())) {
                        ecs.kill_entity(Entity(ecs.get_components<Controllable>().get_index(component)));
                        _net.writeToLogs(logGameInfo, "Entity has been killed.");
                    }
                }
            }

            void affectControllable(Registry &ecs, const std::string &clientUUID, int keyCode)
            {
                auto &conts = ecs.get_components<Controllable>();
                for (auto &cont : conts) {
                    std::string playId(uuidSize, 0);
                    std::memmove(playId.data(), &cont->_playerId, uuidSize);
                    if (cont && !std::strcmp(playId.c_str(), clientUUID.c_str())) {
                        cont->latestInput = keyCode;
                    }
                }
            }

            template <class T>
            void sendSparseArray(const packet::packetTypes &type, sparse_array<T> &sparseArray, const std::string &cliUuid = "")
            {
                _net.sendSparseArray<T>(type, sparseArray, cliUuid);
            }

            bool isServerAvailable()
            {
                return _net.isSocketOpen();
            }

            std::size_t getConnectedNb()
            {
                return _net.getClients().size();
            }

            void handleConnectionRequest(Registry &ecs, packet::packetTypes type)
            {
                std::string clientUUID = _net.addClient();
                std::pair<float, float> pos = newPlayer(ecs, clientUUID);
                packet::connectionRequest data(packet::ACCEPTED, clientUUID, getConnectedNb());
                _net.writeToLogs(logInfo, "UUID of player is " + clientUUID);
                _net.writeToLogs(logInfo, "Connected!");
                _net.sendResponse(packet::CONNECTION_REQUEST, data, true);
                packet::clientStatus cliStatus(clientUUID, packet::NEW_CLIENT, pos.first, pos.second, getConnectedNb());
                try {
                    _net.sendResponse(packet::CLIENT_STATUS, cliStatus);
                } catch (const std::system_error &e) {
                    std::string err = e.what();
                    _net.writeToLogs(logWarn, "Failed to send the response of packet type [" + std::to_string(type) + "]:");
                    _net.writeToLogs(logWarn, "    " + err);
                }
            }

            void handleDisconnectionRequest(Registry &ecs, const std::array<std::uint8_t, packetSize> &packet, const packet::packetHeader &header)
            {
                packet::disconnectionRequest request(packet::ACCEPTED, getConnectedNb());
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
                removePlayer(ecs, clientUUID);
                _net.writeToLogs(logInfo, "Lost connection: Disconnected " + clientUUID);
                packet::clientStatus cliStatus(clientUUID, packet::LOSE_CLIENT, 0.0f, 0.0f, getConnectedNb());
                try {
                    _net.sendResponse(packet::CLIENT_STATUS, cliStatus);
                } catch (const std::system_error &e) {
                    std::string err = e.what();
                    _net.writeToLogs(logWarn, "Failed to send the response of packet type [" + std::to_string(header.type) + "]:");
                    _net.writeToLogs(logWarn, "    " + err);
                }
            }

            void handlekeyboardEvent(Registry &ecs, const std::array<std::uint8_t, packetSize> &packet, const packet::packetHeader &header)
            {
                packet::keyboardEvent event;
                std::memmove(&event, &packet[sizeof(header)], sizeof(event));
                std::string clientUUID(uuidSize, 0);
                std::memmove(clientUUID.data(), &event.uuid, uuidSize);
                affectControllable(ecs, clientUUID, event.keyCode);
            }

            void networkSystemServer(Registry &ecs)
            {
                std::array<std::uint8_t, packetSize> packet;
                packet::packetHeader header;
                std::unordered_map<packet::packetTypes, std::function<void()>> dataHandlers = {
                    {packet::PLACEHOLDER, [&] { _net.writeToLogs(logInfo, "Placeholder received successfully."); }},
                    {packet::CONNECTION_REQUEST, [&] { handleConnectionRequest(ecs, header.type); }},
                    {packet::DISCONNECTION_REQUEST, [&] { handleDisconnectionRequest(ecs, packet, header); }},
                    {packet::KEYBOARD_EVENT, [&] { handlekeyboardEvent(ecs, packet, header); }}
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

            net::Network _net;
    };
}

#endif /* !NETWORKSYSTEM_HPP_ */
