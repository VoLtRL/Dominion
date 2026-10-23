// src/server/RoomManager.cpp
#include "server/RoomManager.hpp"
#include <QDebug>
#include "shared/structs/Payloads.hpp"

RoomManager& RoomManager::getInstance() {
    static RoomManager instance;
    return instance;
}

Room* RoomManager::createRoom(const Network::CreateRoomPayload &payload, Client *client) {
    if (!client) return nullptr;

    if (getRoomByClient(client) != nullptr) {
        leaveRoom(client);
    }

    int roomId = m_nextRoomId++;

    QString roomName;

    if(payload.roomName.isEmpty()){
        roomName = "Room " + roomId;
    } else{
        roomName = payload.roomName;
    }

    auto newRoom = std::make_unique<Room>(roomId, roomName, client);
    Room *roomPtr = newRoom.get();

    m_rooms[roomId] = std::move(newRoom);
    m_clientToRoom[client] = roomPtr;

    qDebug() << "Room created :" << roomId << "(" << roomName << ")";
    return roomPtr;
}

bool RoomManager::joinRoom(Client* client, Room* targetRoom) {
    if (!client) return false;

    if (!targetRoom || targetRoom->isFull() || targetRoom->isGameStarted()) {
        return false;
    }

    if (Room *currentRoom = getRoomByClient(client)) {
        if (currentRoom == targetRoom) return true;
        leaveRoom(client);
    }

    if (targetRoom->addPlayer(client)) {
        m_clientToRoom[client] = targetRoom;
        return true;
    }
    return false;
}

void RoomManager::leaveRoom(Client *client) {
    auto it = m_clientToRoom.find(client);
    if (it == m_clientToRoom.end()) {
        return;
    }

    Room *room = it->second;
    m_clientToRoom.erase(it);

    if (room) {
        room->removePlayer(client);

        if (room->isEmpty()) {
            int roomId = room->getId();
            m_rooms.erase(roomId);
            qDebug() << "Room deleted (empty) :" << roomId;
        }
    }
}

bool RoomManager::startGame(Client *client) {
    Room *room = getRoomByClient(client);
    if (!room) return false;

    return room->startGame(client);
}

Room* RoomManager::getRoomByClient(Client *client) const {
    auto it = m_clientToRoom.find(client);
    return (it != m_clientToRoom.end()) ? it->second : nullptr;
}

Room* RoomManager::getRoomById(int roomId) const {
    auto it = m_rooms.find(roomId);
    return (it != m_rooms.end()) ? it->second.get() : nullptr;
}

std::vector<const Room*> RoomManager::getAvailableRooms() const {
    std::vector<const Room*> availableRooms;
    for (const auto &[id, room] : m_rooms) {
        if (!room->isFull() && !room->isGameStarted()) {
            availableRooms.push_back(room.get());
        }
    }
    return availableRooms;
}