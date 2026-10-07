#include "../../include/gameplay/Game.hpp"

const std::vector<GameMode> Game::gameModes = {
    GameMode("Les premières parties", "Configuration recommandée pour découvrir le jeu", {
        {new Workshop(), 10},
        {new Woodcutter(), 10},
        {new Cellar(), 10},
        {new Moat(), 10},
        {new Smithy(), 10},
        {new Market(), 10},
        {new Militia(), 10},
        {new Mine(), 10},
        {new Remodel(), 10},
        {new Village(), 10}
    }),

    GameMode("Richesses et trésors", "Aventurier, Bureaucrate, Chancelier, Chapelle, Festin, Laboratoire, Marché, Mine, Prêteur sur gages, Salle du Trône", {
        {new Adventurer(), 10},
        {new Bureaucrat(), 10},
        {new Chancellor(), 10},
        {new Chapel(), 10},
        {new Feast(), 10},
        {new Laboratory(), 10},
        {new Market(), 10},
        {new Mine(), 10},
        {new Moneylender(), 10},
        {new ThroneRoom(), 10}
    }),

    GameMode("Interaction", "Bibliothèque, Bureaucrate, Chambre du conseil, Chancelier, Douves, Espion, Festival, Milice, Village, Voleur", {
        {new Library(), 10},
        {new Bureaucrat(), 10},
        {new CouncilRoom(), 10},
        {new Chancellor(), 10},
        {new Moat(), 10},
        {new Spy(), 10},
        {new Festival(), 10},
        {new Militia(), 10},
        {new Village(), 10},
        {new Thief(), 10}
    }),

    GameMode("Changement de taille", "Atelier, Bûcheron, Cave, Chapelle, Festin, Jardins, Laboratoire, Sorcière, Village, Voleur", {
        {new Workshop(), 10},
        {new Woodcutter(), 10},
        {new Cellar(), 10},
        {new Chapel(), 10},
        {new Feast(), 10},
        {new Gardens(), 12},
        {new Laboratory(), 10},
        {new Witch(), 10},
        {new Village(), 10},
        {new Thief(), 10}
    }),

    GameMode("Place du Village", "Bibliothèque, Bûcheron, Bureaucrate, Cave, Festival, Forgeron, Marché, Rénovation, Salle du Trône, Village", {
        {new Library(), 10},
        {new Woodcutter(), 10},
        {new Bureaucrat(), 10},
        {new Cellar(), 10},
        {new Festival(), 10},
        {new Smithy(), 10},
        {new Market(), 10},
        {new Remodel(), 10},
        {new ThroneRoom(), 10},
        {new Village(), 10}
    })
};

Game::Game() {
    
    activePlayer = nullptr;
}

const std::vector<Player>& Game::getPlayers() const {
    return players;
}

const Player& Game::getActivePlayer() const {
    return *activePlayer;
}

void Game::addPlayer(Player player) {
    players.push_back(player);
}

void Game::setActivePlayer(Player* player) {
    activePlayer = player;
}

void Game::setCurrentGameMode(const GameMode* gameMode) {
    currentGameMode = gameMode;
}

const GameMode* Game::getCurrentGameMode() const {
    return currentGameMode;
}

void Game::playTurn(Player player) {
    setActivePlayer(&player);
}
