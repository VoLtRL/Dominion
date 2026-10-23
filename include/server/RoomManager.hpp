#pragma once

#include <unordered_map>
#include <memory>
#include <QString>
#include <QJsonObject>
#include "server/Room.hpp"
#include "server/Client.hpp"

class RoomManager {
private:
    RoomManager() = default;

    int m_nextRoomId{1};
    std::unordered_map<int, std::unique_ptr<Room>> m_rooms;
    std::unordered_map<Client*, Room*> m_clientToRoom;

public:
    static RoomManager& getInstance();

    RoomManager(const RoomManager&) = delete;
    RoomManager& operator=(const RoomManager&) = delete;

    Room* createRoom(const Network::CreateRoomPayload &payload, Client *client);
    bool joinRoom(Client* client, Room* targetRoom);
    void leaveRoom(Client *client);
    bool startGame(Client *client);

    Room* getRoomByClient(Client *client) const;
    Room* getRoomById(int roomId) const;
    std::vector<const Room*> getAvailableRooms() const;


};