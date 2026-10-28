#pragma once

#include <vector>
#include <memory>
#include <optional>
#include <QString>
#include "server/Client.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/gameplay/GameMode.hpp"
#include "shared/structs/Payloads.hpp"

class Room {
public:
    Room(int id, const QString &name, Client *host);
    ~Room() = default;

    Network::GameStatePayload getGameStatePayload(const Client *client) const;

    int getId() const { return m_id; }
    QString getName() const { return m_name; }
    Client* getHost() const { return m_host; }
    const std::vector<Client*>& getClients() const { return m_players; }
    Game* getGame() const { return m_game.get(); }

    bool isEmpty() const { return m_players.empty(); }
    bool isFull() const { return m_players.size() >= 4; }
    bool isGameStarted() const { return m_game != nullptr; }

    void setGameMode(const GameMode &gameMode);

    int getPlacesLeft() const { return 4 - m_players.size(); }
    const GameMode* getGameMode() const {
        return chosenGameMode ? &*chosenGameMode : nullptr;
    }

    bool addPlayer(Client *player);
    void removePlayer(Client *player);
    bool startGame(Client *requester);

private:
    int m_id;
    QString m_name;
    Client *m_host{nullptr};
    std::vector<Client*> m_players;
    std::unique_ptr<Game> m_game{nullptr};
    std::optional<GameMode> chosenGameMode;
};