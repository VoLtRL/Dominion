#pragma once

#include <vector>
#include <memory>
#include <QString>
#include "server/Client.hpp"
#include "shared/gameplay/Game.hpp"

class Room {
public:
    Room(int id, const QString &name, Client *host);
    ~Room() = default;

    int getId() const { return m_id; }
    QString getName() const { return m_name; }
    Client* getHost() const { return m_host; }
    const std::vector<Client*>& getClients() const { return m_players; }
    Game* getGame() const { return m_game.get(); }

    bool isEmpty() const { return m_players.empty(); }
    bool isFull() const { return m_players.size() >= 4; }
    bool isGameStarted() const { return m_game != nullptr; }

    int getPlacesLeft() const { return 4 - m_players.size(); }

    bool addPlayer(Client *player);
    void removePlayer(Client *player);
    bool startGame(Client *requester);

private:
    int m_id;
    QString m_name;
    Client *m_host{nullptr};
    std::vector<Client*> m_players;
    std::unique_ptr<Game> m_game{nullptr};
};