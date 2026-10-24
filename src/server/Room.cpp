#include "server/Room.hpp"
#include <algorithm>

Room::Room(int id, const QString &name, Client *host)
    : m_id(id), m_name(name), m_host(host) {
    if (host) {
        m_players.push_back(host);
    }
}

bool Room::addPlayer(Client *player) {
    if (!player || isFull() || isGameStarted()) {
        return false;
    }
    auto it = std::find(m_players.begin(), m_players.end(), player);
    if (it != m_players.end()) {
        return false;
    }

    m_players.push_back(player);
    return true;
}

void Room::removePlayer(Client *player) {
    m_players.erase(
        std::remove(m_players.begin(), m_players.end(), player),
        m_players.end()
    );

    // if the removed player was the host, assign a new host if there are still players left
    if (m_host == player) {
        m_host = m_players.empty() ? nullptr : m_players.front();
    }
}

bool Room::startGame(Client *requester) {
    // only the host can start the game, and there must be at least 2 players, and the game must not have already started
    if (requester != m_host || m_players.size() < 2 || isGameStarted()) {
        return false;
    }

    m_game = std::make_unique<Game>();
    m_game->launchGame();
    return true;
}