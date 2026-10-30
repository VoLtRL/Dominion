    #include "server/Room.hpp"
#include <algorithm>
#include "shared/structs/GamePhase.hpp"

Room::Room(int id, const QString &name, Client *host)
    : m_id(id), m_name(name), m_host(host) {
    if (host) {
        m_players.push_back(host);
    }
}

Network::GameStatePayload Room::getGameStatePayload(const Client *client) const {
    const Player *self = client ? client->getAssociatedPlayer() : nullptr;
    if (!m_game || !self) {
        return Network::GameStatePayload("", "", GamePhase::UNKNOWN, 0, 0, 0, Hand(), {}, {}, {});
    }

    std::vector<Network::PlayerState> players;
    for (const Client *roomClient : m_players) {
        if (!roomClient->getAssociatedPlayer()) {
            continue;
        }

        const Player *player = roomClient->getAssociatedPlayer();
        const auto &discardStack = player->getDiscardStack();
        players.push_back(Network::PlayerState(
            player->getName().c_str(),
            static_cast<int>(player->getHand().getSize()),
            player->getDrawStack().getSize(),
            discardStack.getSize(),
            discardStack.getStack().empty() ? nullptr : discardStack.getStack().top(),
            player->getCardsInPlay(),
            player == &m_game->getActivePlayer()));
    }

    Network::GameStatePayload payload(
        self->getName().c_str(), // selfName
        m_game->getActivePlayer().getName().c_str(), // activePlayerName
        m_game->getCurrentPhase(), // currentPhase
        m_game->getCurrentPlayerActions(), // actions
        m_game->getCurrentPlayerPurchasesLeft(), // buys
        m_game->getCurrentPlayerMoney(), // money
        self->getHand(), // hand
        self->getCardsInPlay(), // cardsInPlay
        m_game->getCurrentCardDistribution(), // cardDistribution
        players
    );
    return payload;
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

void Room::setGameMode(const GameMode &gameMode) {
    chosenGameMode = gameMode;
}

bool Room::startGame(Client *requester) {
    // only the host can start the game, and there must be at least 2 players, and the game must not have already started
    if (requester != m_host || m_players.size() < 2 || isGameStarted()) {
        return false;
    }

    m_game = std::make_unique<Game>();
    std::vector<QString> playerNames;
    for (const Client *player : m_players) {
        playerNames.push_back(player->getName());
    }
    m_game->initialize(*chosenGameMode, playerNames);
    for (Client *player : m_players) {
        player->setAssociatedPlayer(m_game->getPlayerByName(player->getName().toStdString()));
    }
    return true;
}